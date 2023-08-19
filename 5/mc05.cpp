//コンパイルコマンド　g++ mc05.cpp -std=c++11 `pkg-config --cflags --libs opencv4` && ./a.out
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
// #include "../Histogram.h"

//関数マクロ
#define toRad(x) (x*CV_PI/180.0)
#define toDeg(x) (x*180.0/CV_PI)

void applyLUT(cv::Mat src, cv::Mat &dst, std::vector<int> LUT)
{
	for(int j=0; j<src.rows; j++){
		for(int i=0; i<src.cols; i++){
			unsigned char x = src.at<unsigned char>(j,i);
			dst.at<unsigned char>(j,i) = LUT[x];
		}
	}
}

void applyLUT(cv::Mat src, cv::Mat &dst, std::vector<int> LUT0, std::vector<int> LUT1, std::vector<int> LUT2)
{
	if(src.channels() != 3 || dst.channels() != 3){
		printf("src&dst must be a color image!\n");
		return;
	}
	for(int j=0; j<src.rows; j++){
		for(int i=0; i<src.cols; i++){
			cv::Vec3b x = src.at<cv::Vec3b>(j,i);
			x[0] = LUT0[x[0]];
			x[1] = LUT1[x[1]];
			x[2] = LUT2[x[2]];
			dst.at<cv::Vec3b>(j,i) = x;
		}
	}
}


int main (int argc, const char* argv[])
{
    //内蔵カメラからビデオキャプチャ"capture"生成
    cv::VideoCapture capture(0);  //"water.mov"にすればムービーファイルから映像を取り込む
    //キャプチャできたかチェック
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }
	
    //フレーム格納用画像
    cv::Mat frameImage;
	//ウィンドウの生成
	cv::namedWindow("Frame");

    //ビデオキャプチャから1フレーム取り出し
    capture >> frameImage;
	//画像サイズの縮小
	cv::resize(frameImage, frameImage, cv::Size(), 0.5, 0.5);
//	cv::resize(frameImage, frameImage, cv::Size(frameImage.cols/2, frameImage.rows/2));

	//処理結果画像
	cv::Mat grayImage = cv::Mat(frameImage.size(), CV_8UC1);
	cv::Mat negaImage = cv::Mat(frameImage.size(), CV_8UC1);
	cv::Mat transImage = cv::Mat(frameImage.size(), CV_8UC1);
	cv::Mat embossImage = cv::Mat(frameImage.size(), CV_8UC1);
	cv::Mat trajectoryImage = cv::Mat(frameImage.size(), CV_8UC3);
	cv::Mat resultImage = cv::Mat(frameImage.size(), CV_8UC3);
    cv::Mat grayColorImage = cv::Mat(frameImage.size(), CV_8UC3);
    cv::Mat hsvImage = cv::Mat(frameImage.size(), CV_8UC3);
    cv::Mat prevImage = cv::Mat(frameImage.size(), CV_8UC3);

	//ウィンドウの生成
	cv::namedWindow("Negaposi");
	cv::moveWindow("Negaposi", frameImage.cols, 0);
	cv::namedWindow("Translation");
	cv::moveWindow("Translation", 0, frameImage.rows);
	cv::namedWindow("Result");
	cv::moveWindow("Result", frameImage.cols, frameImage.rows);

	//LUTテーブルの用意
	std::vector<int> LUT(256), LUT2X(256), LUTgamma(256), LUTsigmoid(256), LUTnega(256), LUTcontrast(256), LUT_B(256), LUT_G(256), LUT_R(256), LUT_H(256), LUT_S(256), LUT_V(256);

	//y=2xのトーンカーブ
	int y;
	for(int x=0; x<LUT.size(); x++){
		//x:x
		//y:LUT[x]
		//LUT2X y=2x
		y = 2*x;
		if(y > 255) y = 255;
		if(y < 0) y = 0;
		LUT2X[x] = y;
		//LUTgamma y=255*(x/255)^(1/r) r = 2
		double r = 2.0;
		y = (int)255*pow((x/255.0), 1/r);
		if(y > 255) y = 255;
		if(y < 0) y = 0;
		LUTgamma[x] = y;
		//LUTsigmoid y=1/(1+e^(-a(x-x0)*256)
		double a = 0.02;
		int x0 = 127;
		y = (int)(1.0/(1.0+exp(-a*(x-x0)))*256);
		if(y > 255) y = 255;
		if(y < 0) y = 0;
		LUTsigmoid[x] = y;
		//LUTnega y=255-x
		y = 255-x;
		if(y > 255) y = 255;
		if(y < 0) y = 0;
		LUTnega[x] = y;

        // LUTcontrast y = 2*x-128
        y = 2*x - 128;
        if(y>255)y=255;
        if(y<0)y=0;
        LUTcontrast[x]=y;

        //擬似カラー
        //B
        y = -4*x + 512;
        if(y>255)y=255;
        if(y<0)y=0;
        LUT_B[x] = y;

        //G
        if(x<64){
            y=4*x;
        }else if(64<=x && x<192){
            y = 255;
        }else{
            y = -4*x + 1024;
        }
        LUT_G[x] = y;

        //R
        y = 4*x - 512;
        if(y>255)y=255;
        if(y<0)y=0;
        LUT_R[x]=y;

        //古い写真風
        LUT_H[x]=x;
        LUT_S[x]=x/2;
        LUT_V[x]=x/1.5;
	}
	
	//グレースケールに変換
	cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);

	applyLUT(grayImage, negaImage, LUTnega);

    prevImage = frameImage.clone();
	
    int cnt = 0;

	while(1){
        cnt+=8;
		capture >> frameImage;
		cv::resize(frameImage, frameImage, cv::Size(), 0.5, 0.5);

		//グレースケールに変換
		cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);
        cv::cvtColor(grayImage, grayColorImage, cv::COLOR_GRAY2BGR);
        cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);
		
		//トーンカーブによる変換
		applyLUT(grayImage, negaImage, LUTnega);
        applyLUT(grayColorImage, resultImage, LUT_B, LUT_G, LUT_R);
        applyLUT(frameImage, resultImage, LUTcontrast, LUTcontrast, LUTcontrast);
        applyLUT(hsvImage, resultImage, LUT_H, LUT_S, LUT_V);
        cv::cvtColor(resultImage, resultImage, cv::COLOR_HSV2BGR);
        cv::addWeighted(prevImage, 0.5, frameImage, 0.5, 0, resultImage);

        // 並行移動
        for(int j=0; j<negaImage.rows; j++){
            for(int i=0; i<negaImage.cols; i++){
                if(i < negaImage.cols-3 && j < negaImage.rows-3){
                    unsigned char x = negaImage.at<unsigned char>(j,i);
                    transImage.at<unsigned char>(j+3,i+3) = x;   
                }
            }
        }
        //足し合わせ
        cv::addWeighted(grayImage, 1, transImage, 1, -128, embossImage);
		cv::addWeighted()

		//画像表示
		cv::imshow("Frame", frameImage);
		cv::imshow("Negaposi", grayImage);
		cv::imshow("Translation", embossImage);
		cv::imshow("Result", resultImage);

        if(cnt%10){
            prevImage = frameImage.clone();
        }

		char key = cv::waitKey(10);
		if(key == 'q' || key == 'Q') break;
	}
	
    //キー入力待ち
//    cv::waitKey(0);
    return 0;
}