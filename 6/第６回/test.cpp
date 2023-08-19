#include <opencv2/opencv.hpp>

int main() {
    // 動画のキャプチャ用オブジェクトを作成
    cv::VideoCapture cap(0);  // カメラデバイスのインデックスを指定 (0はデフォルトのカメラ)

    // カメラが正常にオープンできたかをチェック
    if (!cap.isOpened()) {
        std::cerr << "Failed to open camera." << std::endl;
        return -1;
    }

    // ウィンドウを作成
    cv::namedWindow("Video", cv::WINDOW_NORMAL);
    cv::namedWindow("Gray", cv::WINDOW_NORMAL);
    cv::namedWindow("Canny", cv::WINDOW_NORMAL);

    // ウィンドウのリサイズ
    cv::resizeWindow("Video", 400, 300);
    cv::resizeWindow("Gray", 400, 300);
    cv::resizeWindow("Canny", 400, 300);

    while (true) {
        cv::Mat frame;
        cap >> frame;  // フレームをキャプチャ

        // カメラからのフレームキャプチャが正常であるかをチェック
        if (frame.empty()) {
            std::cerr << "Failed to capture frame." << std::endl;
            break;
        }

        // グレースケールに変換
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Cannyエッジ検出を実行
        cv::Mat edges;
        cv::Canny(gray, edges, 50, 150);

        // ウィンドウにフレームとエッジを表示
        cv::imshow("Video", frame);
        cv::imshow("Gray", gray);
        cv::imshow("Canny", edges);

        // 'q'キーが押されたら終了
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // ウィンドウを破棄して終了
    cv::destroyAllWindows();
    return 0;
}
