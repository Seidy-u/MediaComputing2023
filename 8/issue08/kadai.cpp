#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // 画像の読み込み
    cv::Mat image = cv::imread("keyboard.png", cv::IMREAD_GRAYSCALE);

    // 二値化処理
    cv::Mat binaryImage;
    cv::threshold(image, binaryImage, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    // 輪郭検出
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // フィルタリング処理：小さな輪郭を削除
    std::vector<std::vector<cv::Point>> filteredContours;
    int minContourArea = 50; // フィルタリングする輪郭の最小面積
    for (const auto& contour : contours) {
        double contourArea = cv::contourArea(contour);
        if (contourArea >= minContourArea) {
            filteredContours.push_back(contour);
        }
    }

    // キーの個数を出力
    int keyCount = filteredContours.size();
    std::cout << "キーの個数: " << keyCount << std::endl;

    return 0;
}
