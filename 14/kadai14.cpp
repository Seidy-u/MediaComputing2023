#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
    VideoCapture cap(0); // PC内蔵カメラの入力を受け付ける
    if (!cap.isOpened()) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    namedWindow("MeanShift Tracking", WINDOW_AUTOSIZE);

    Rect trackWindow(200, 200, 200, 200); // 初期段階で画面中央に200x200の四角形を表示

    bool tracking = false;
    Mat frame, hsv, mask, hist, backProj;

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            break;
        }

        if (tracking) {
            // フレームをHSVカラースペースに変換
            cvtColor(frame, hsv, COLOR_BGR2HSV);

            // バックプロジェクションを計算
            calcBackProject(&hsv, 1, 0, hist, backProj, &trackWindow);

            // ミーンシフトを適用して物体の新しい位置を取得
            meanShift(backProj, trackWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

            // 物体を囲む黄色い四角形を描画
            rectangle(frame, trackWindow, Scalar(0, 255, 255), 2);
        } else {
            // 物体追跡を開始するために'R'キーを押下するまで、中央に四角形を描画
            rectangle(frame, trackWindow, Scalar(0, 0, 255), 2);
        }

        imshow("MeanShift Tracking", frame);

        char c = waitKey(1);
        if (c == 'r' || c == 'R') {
            // 'R'キーを押下した場合、物体追跡を開始
            tracking = true;

            // 四角形内の領域をROIとして抽出し、そのヒストグラムを計算
            Mat roi(hsv, trackWindow);
            calcHist(&roi, 1, 0, Mat(), hist, 1, &histSize, &histRange, true);
            normalize(hist, hist, 0, 255, NORM_MINMAX);
        } else if (c == 'q' || c == 'Q') {
            // 'Q'キーを押下した場合、物体追跡を終了
            tracking = false;
        } else if (c == 27) {
            // 'Esc'キーを押下した場合、プログラムを終了
            break;
        }
    }

    cap.release();
    destroyAllWindows();

    return 0;
}
