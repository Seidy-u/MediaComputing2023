//  g++ car_task11.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
//  g++ car_task11.cpp `pkg-config --cflags --libs opencv`

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char* argv[])
{
    cv::VideoCapture video("car_movie.mp4");
    cv::Mat templateImage = cv::imread("car_img.png");
    cv::Mat frame;

    if (!video.isOpened() || templateImage.empty())
    {
        printf("File not found\n");
        exit(0);
    }

    // ORB特徴点検出と特徴量計算
    // ORB特徴点検出機器と特徴量計算機を作成
    cv::Ptr<ORB> orb = ORB::create(50);
    
    // 検出された特徴点を格納
    std::vector<cv::KeyPoint> templateKeypoints, frameKeypoints;
    
    // 計算された特徴量を格納
    cv::Mat templateDescriptors, frameDescriptors;
    
    // 2つの特徴量のインデックス・マッチングの距離・およびその他の情報を保持するデータ
    std::vector<cv::DMatch> matches;
    
    // 画像の特徴点と特徴量を計算
    orb->detectAndCompute(templateImage, cv::noArray(), templateKeypoints, templateDescriptors);
    
    // Brute-Forceクラスのオブジェクトを生成
    cv::BFMatcher matcher(NORM_HAMMING);
    
    while (true)
    {
        // 動画の再生位置を最初に戻す
        video.set(cv::CAP_PROP_POS_FRAMES, 0);
    
        while (video.read(frame))
        {
            // 動画の特徴点と特徴量を計算
            orb->detectAndCompute(frame, cv::noArray(), frameKeypoints, frameDescriptors);

            // matcher.match関数を使って2つの特徴量セットをマッチングし，matchesに格納
            matcher.match(templateDescriptors, frameDescriptors, matches);

            // 画像と動画をマッチング情報をもとに横に並べて表示し，対応する点で結ぶ
            cv::Mat result;
            cv::drawMatches(templateImage, templateKeypoints, frame, frameKeypoints, matches, result);

            // マッチング結果を表示
            cv::imshow("Matching", result);
        
            char key = cv::waitKey(1);
        
            if(key == 'q' || key == 'Q') {
            return 0;
            }
        }
    }

    video.release();

    return 0;
}
