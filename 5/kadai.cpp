#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture capture(0);
    if (!capture.isOpened())
    {
        std::cout << "Failed to open the camera." << std::endl;
        return -1;
    }

    capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::Mat frame, grayFrame;
    cv::Mat previousFrame, currentFrame;
    cv::Mat frameDiff, thresholdImage;

    cv::namedWindow("Motion Detection");

    cv::Mat overlayImage = cv::imread("brasil.jpeg", cv::IMREAD_UNCHANGED);

    while (true)
    {
        capture >> frame;

        if (frame.empty())
        {
            break;
        }

        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        if (previousFrame.empty())
        {
            previousFrame = grayFrame.clone();
        }

        cv::absdiff(grayFrame, previousFrame, frameDiff);
        cv::threshold(frameDiff, thresholdImage, 30, 255, cv::THRESH_BINARY);

        cv::imshow("Motion Detection", frameDiff);

        cv::Mat modifiedFrame = frame.clone();

        // 動体の領域に画像を重ねる
        for (int i = 0; i < modifiedFrame.rows; i++)
        {
            for (int j = 0; j < modifiedFrame.cols; j++)
            {
                if (thresholdImage.at<uchar>(i, j) > 0)
                {
                    // 画像のアルファチャンネルを考慮して重ね合わせる
                    cv::Vec4b overlayPixel = overlayImage.at<cv::Vec4b>(i % overlayImage.rows, j % overlayImage.cols);
                    float alpha = overlayPixel[3] / 255.0;

                    modifiedFrame.at<cv::Vec3b>(i, j)[0] = modifiedFrame.at<cv::Vec3b>(i, j)[0] * (1 - alpha) + overlayPixel[0] * alpha;
                    modifiedFrame.at<cv::Vec3b>(i, j)[1] = modifiedFrame.at<cv::Vec3b>(i, j)[1] * (1 - alpha) + overlayPixel[1] * alpha;
                    modifiedFrame.at<cv::Vec3b>(i, j)[2] = modifiedFrame.at<cv::Vec3b>(i, j)[2] * (1 - alpha) + overlayPixel[2] * alpha;
                }
            }
        }

        cv::imshow("Modified Frame", modifiedFrame);

        previousFrame = grayFrame.clone();

        char key = cv::waitKey(30);
        if (key == 'q')
        {
            break;
        }
    }

    capture.release();

    cv::destroyAllWindows();

    return 0;
}
