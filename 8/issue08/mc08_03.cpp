//g++ mc08_03.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>
#include "Stats.h"

int main()
{
    // 画像の読み込み
    cv::Mat img = cv::imread("input_image_with_shapes.png", cv::IMREAD_GRAYSCALE);
    if (img.empty())
    {
        std::cout << "画像ファイルを読み込めませんでした。" << std::endl;
        return -1;
    }

    // ヒストグラムの生成
    cv::Mat thresh_img;
    cv::threshold(img, thresh_img, 10, 256, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    // 膨張収縮処理

    //画像の表示
    cv::imshow("Source Image", img);
    //cv::imshow("Thresholded Image", thresh_img);
    cv::waitKey();
    // ヒストグラムの表示

    return 0;
}
