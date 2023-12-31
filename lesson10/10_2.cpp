// g++ 10_2.cpp -std=c++11 `pkg-config --cflags --libs opencv4` && ./a.out
#include <opencv2/opencv.hpp>

int main(int argc, const char *argv[])
{
  // 画像読み込み
  cv::Mat sourceImage = cv::imread("images/aquarium.jpg");
  cv::Mat templateImage = cv::imread("images/fish.jpg");

  cv::Mat graySourceImage, grayTemplateImage;
  cv::Mat binarySourceImage, binaryTemplateImage;
  cv::Mat edgeSourceImage, edgeTemplateImage;
  cv::Mat distSourceImage;
  cv::Mat normalizedSourceImage;

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

  // 入力画像の距離変換画像を作成
  // 画像のグレースケール化&二値化
  cv::cvtColor(sourceImage, graySourceImage, cv::COLOR_BGR2GRAY);
  cv::threshold(graySourceImage, binarySourceImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

  // ソーベルフィルタでエッジ抽出
  cv::Sobel(binarySourceImage, edgeSourceImage, -1, 1, 1, 3);

  // 白黒反転
  cv::bitwise_not(edgeSourceImage, edgeSourceImage);

  // 距離変換
  cv::distanceTransform(edgeSourceImage, distSourceImage, cv::DIST_L2, 5);

  // 正規化
  cv::normalize(distSourceImage, normalizedSourceImage, 0.0, 255.0, cv::NORM_MINMAX, 0);

  // テンプレート画像のエッジ画像を作成

  // 画像のグレースケール化&二値化
  cv::cvtColor(templateImage, grayTemplateImage, cv::COLOR_BGR2GRAY);
  cv::threshold(grayTemplateImage, binaryTemplateImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

  // ソーベルフィルタでエッジ抽出
  cv::Sobel(binaryTemplateImage, edgeTemplateImage, -1, 1, 1, 3);

  // 距離変換画像とテンプレート画像のエッジ画像でチャンファーマッチング
  cv::matchTemplate(normalizedSourceImage, edgeSourceImage, resultImage, cv::TM_CCORR);
  cv::minMaxLoc(resultImage, &val, NULL, &matchPoint, NULL);

  cv::imshow("Source", sourceImage);
  cv::imshow("Template", templateImage);
  cv::imshow("EdgeSource", edgeSourceImage);
  cv::imshow("NormalizedSource", normalizedSourceImage);
  cv::imshow("EdgeTemplate", edgeTemplateImage);

  cv::waitKey(0);

  return 0;
}
