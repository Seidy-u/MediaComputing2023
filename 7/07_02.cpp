// g++ 07_02.cpp -std=c++11 `pkg-config --cflags --libs opencv4` && ./a.out
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>

int main(int argc, const char* argv[]) {
    // 画像の読み込み
    cv::Mat Source1 = cv::imread("A.png", cv::IMREAD_COLOR);
    if (Source1.empty()) {
        std::cout << "Failed to open Source image1" << std::endl;
        return -1;
    }

    cv::Mat Source2 = cv::imread("B.png", cv::IMREAD_COLOR);
    if (Source2.empty()) {
        std::cout << "Failed to open Source image2" << std::endl;
        return -1;
    }

    cv::Mat Source3 = cv::imread("C.png", cv::IMREAD_COLOR);
    if (Source3.empty()) {
        std::cout << "Failed to open Source image3" << std::endl;
        return -1;
    }

    // 画像を配列に格納
    std::vector<cv::Mat> Sources;
    Sources.push_back(Source1);
    Sources.push_back(Source2);
    Sources.push_back(Source3);

    // A-KAZE検出器の生成
    cv::Ptr<cv::Feature2D> akaze = cv::AKAZE::create();

    // 特徴点と特徴記述子を計算
    std::vector<std::vector<cv::KeyPoint>> keypoints;
    std::vector<cv::Mat> descriptors;

    for (const cv::Mat& source : Sources) {
        std::vector<cv::KeyPoint> kp;
        cv::Mat des;
        akaze->detectAndCompute(source, cv::noArray(), kp, des);
        keypoints.push_back(kp);
        descriptors.push_back(des);

        // 特徴点を描画した画像を作成
        cv::Mat img_with_keypoints;
        cv::drawKeypoints(source, kp, img_with_keypoints);

        // 画像を表示
        // cv::imshow("Image with Keypoints", img_with_keypoints);
        // cv::waitKey(0);
    }

    // 特徴点のマッチングを行い、マッチング結果を描画
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce");
    std::vector<std::vector<cv::DMatch>> matches;
    matcher->knnMatch(descriptors[0], descriptors[1], matches, 2);

    std::vector<cv::DMatch> good_matches;
    float ratio = 0.8f;
    for (size_t i = 0; i < matches.size(); i++) {
        if (matches[i][0].distance < ratio * matches[i][1].distance) {
            good_matches.push_back(matches[i][0]);
        }
    }

    cv::Mat img_matches;
    cv::drawMatches(Source1, keypoints[0], Source2, keypoints[1], good_matches, img_matches);

    // マッチング結果を表示
    cv::imshow("Matches", img_matches);
    // cv::waitKey(0);

    // パノラマ画像の生成用インスタンスを生成
    cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA;
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(mode);

    // 出力画像の宣言
    cv::Mat panorama;

    // パノラマ画像の生成
    cv::Stitcher::Status status = stitcher->stitch(Sources, panorama);

    if (status == cv::Stitcher::OK) {
        // 画像を表示
        cv::imshow("Panorama", panorama);
        cv::waitKey(0);
    } else {
        std::cout << "Failed to stitch images" << std::endl;
        return -1;
    }

    return 0;
}