// g++ 10_1.cpp -std=c++11 `pkg-config --cflags --libs opencv4` && ./a.out
#include <opencv2/opencv.hpp>

int main(int argc, const char *argv[])
{
  // 画像読み込み
  cv::Mat sourceImage = cv::imread("images/find_wally.jpg");
  cv::Mat templateImage = cv::imread("images/wally.png");

  if (sourceImage.empty() || templateImage.empty())
  {
    printf("File not found\n");
    exit(0);
  }

  // 出力画像
  cv::Mat resultImage(cv::Size(sourceImage.cols, sourceImage.rows), CV_8UC3);
  cv::Point matchPoint;
  cv::Rect resultRect(0, 0, templateImage.cols, templateImage.rows);
  double val;

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

  cv::imshow("Source", sourceImage);
  cv::imshow("Template", templateImage);
  cv::imshow("Result", resultImage);

  cv::waitKey(0);

  return 0;
}
