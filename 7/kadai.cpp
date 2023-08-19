#include <opencv2/opencv.hpp>

int main() {
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) {
        std::cout << "Failed to open the camera." << std::endl;
        return -1;
    }

    cv::Mat frame;
    capture >> frame;

    // ウィンドウの設定
    cv::namedWindow("Tunnel Effect", cv::WINDOW_NORMAL);
    cv::resizeWindow("Tunnel Effect", frame.cols, frame.rows);

    // 変換行列の作成
    cv::Mat warpMatrix(2, 3, CV_32F);
    warpMatrix.at<float>(0, 0) = 1.0;
    warpMatrix.at<float>(0, 1) = 0.5;
    warpMatrix.at<float>(0, 2) = 0.0;
    warpMatrix.at<float>(1, 0) = 0.0;
    warpMatrix.at<float>(1, 1) = 1.0;
    warpMatrix.at<float>(1, 2) = 0.0;

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            std::cout << "Failed to capture frame." << std::endl;
            break;
        }

        // 画像の変形
        cv::Mat result;
        cv::warpAffine(frame, result, warpMatrix, frame.size());

        // ウィンドウに表示
        cv::imshow("Tunnel Effect", result);

        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
