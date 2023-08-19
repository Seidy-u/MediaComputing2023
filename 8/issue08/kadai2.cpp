#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // 画像の読み込み
    cv::Mat image = cv::imread("keyboard.png", cv::IMREAD_COLOR);

    // 二値化処理
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    cv::Mat binaryImage;
    cv::threshold(grayImage, binaryImage, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

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

    // キーの個数と位置を出力
    int keyCount = filteredContours.size();
    std::cout << "キーの個数: " << keyCount << std::endl;

    cv::Scalar keyColor(0, 0, 255); // キーの表示色 (赤)

    for (int i = 0; i < keyCount; ++i) {
        // キーの輪郭を描画
        cv::drawContours(image, filteredContours, i, keyColor, 2);

        // キーの重心座標を計算
        cv::Moments moments = cv::moments(filteredContours[i]);
        int centerX = static_cast<int>(moments.m10 / moments.m00);
        int centerY = static_cast<int>(moments.m01 / moments.m00);

        // キーの位置を表示
        cv::putText(image, std::to_string(i + 1), cv::Point(centerX, centerY),
                    cv::FONT_HERSHEY_SIMPLEX, 1, keyColor, 2);
    }

    cv::imshow("Keyboard", image);
    cv::waitKey(0);

    return 0;
}
