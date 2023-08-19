// g++ 10_1.cpp -std=c++11 `pkg-config --cflags --libs opencv4` && ./a.out
#include <opencv2/opencv.hpp>

int main(int argc, const char *argv[])
{
    std::string filepath = "images/game.mp4";
	cv::VideoCapture video;
	video.open(filepath);
	if (video.isOpened() == false) {
		exit(0);
	}
	cv::VideoWriter writer;
    
	int width, height, fourcc;
	fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
	double fps;
	width  = (int)video.get(cv::CAP_PROP_FRAME_WIDTH);
	height = (int)video.get(cv::CAP_PROP_FRAME_HEIGHT);
	fps    = video.get(cv::CAP_PROP_FPS);

	writer.open("submit.mp4", fourcc, fps, cv::Size(width, height));
    cv::Mat sourceImage;

	cv::Mat templateImage = cv::imread("images/player.png");
	if (templateImage.empty())
	{
    	printf("File not found\n");
	    exit(0);
	}
	// 出力画像
	cv::Mat resultImage(cv::Size(sourceImage.cols, sourceImage.rows), CV_8UC3);
	cv::Point matchPoint;
	cv::Rect resultRect(0, 0, templateImage.cols, templateImage.rows);
	double val;

	while (1) {
		video >> sourceImage;
		if (sourceImage.empty() == true) break;

		// テンプレートマッチング
		//SSD
		cv::matchTemplate(sourceImage, templateImage, resultImage, cv::TM_SQDIFF);
		//ZNCC
		// cv::matchTemplate(sourceImage, templateImage, resultImage, cv::TM_CCOEFF_NORMED);

		// 最小値とその位置を取得
		cv::minMaxLoc(resultImage, &val, NULL, &matchPoint, NULL);

		// マッチング位置の指定
		resultRect.x = matchPoint.x;
		resultRect.y = matchPoint.y;

		// 矩形の描画
		cv::rectangle(sourceImage, resultRect, cv::Scalar(0, 0, 255), 3);

		cv::imshow("showing", sourceImage);
		writer << sourceImage;
		if (cv::waitKey(1) == 'q') break;
	}

	return 0;
}
