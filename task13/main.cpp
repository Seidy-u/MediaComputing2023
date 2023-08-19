// g++ main.cpp -std=c++11 `pkg-config --cflags --libs opencv4` -std=c++17 && ./a.out
#include <iostream> //入出力関連ヘッダ
#include <vector>
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, const char *argv[])
{
    // 内蔵カメラからビデオキャプチャ"capture"生成
    cv::VideoCapture capture(0); //"water.mov"にすればムービーファイルから映像を取り込む
    // キャプチャできたかチェック
    if (capture.isOpened() == 0)
    {
        printf("Camera not found\n");
        return -1;
    }

    // Haar-like特徴分類器の読み込み
    cv::CascadeClassifier face_cascade;
    face_cascade.load("./haarcascade/haarcascade_frontalface_default.xml");

    // フレーム格納用画像
    cv::Mat frameImage;

    // ビデオキャプチャから1フレーム取り出し
    capture >> frameImage;
    // 画像サイズの縮小
    cv::resize(frameImage, frameImage, cv::Size(), 0.5, 0.5);
    //	cv::resize(frameImage, frameImage, cv::Size(frameImage.cols/2, frameImage.rows/2));
    int prevsize = 5;

    // 処理結果画像
    cv::Mat grayImage = cv::Mat(frameImage.size(), CV_8UC1);
    // グレースケール変換
    cv::Mat gray;
    cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);

    // ウィンドウの生成
    cv::namedWindow("Result");
    cv::moveWindow("Result", frameImage.cols, 0);

    int cnt = 0;
    while (1)
    {
        capture >> frameImage;
        cv::resize(frameImage, frameImage, cv::Size(), 0.5, 0.5);

        // グレースケールに変換
        cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);

        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(grayImage, faces);
        for (size_t i = 0; i < faces.size(); i++)
        {
            // 検知した顔を矩形で囲む
            cv::rectangle(frameImage, faces[i], cv::Scalar(255, 0, 0), 2);
        }

        // 画像表示
        cv::imshow("Result", frameImage);

        char key = cv::waitKey(10);
        if (key == 'q' || key == 'Q')
            break;
    }

    return 0;
}