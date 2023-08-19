#include <stdio.h>
#include <opencv2/opencv.hpp>

void drawOpticalFlow(cv::Mat& result, cv::Mat flow)
{
    cv::Mat flow_planes[2];
    cv::split(flow, flow_planes);

    cv::Mat magnitude, angle, hsvSplit[3], hsvImage, hsvImage8, bgrFlow;

    cv::cartToPolar(flow_planes[0], flow_planes[1], magnitude, angle, true);

    hsvSplit[0] = angle;
    normalize(magnitude, magnitude, 0, 1, cv::NORM_MINMAX);
    hsvSplit[2] = magnitude;

    hsvSplit[1] = cv::Mat::ones(angle.size(), CV_32F);

    cv::merge(hsvSplit, 3, hsvImage);

    hsvImage.convertTo(hsvImage8, CV_8U, 255.0);
    cv::cvtColor(hsvImage8, bgrFlow, cv::COLOR_HSV2BGR);

    cv::addWeighted(result, 1, bgrFlow, 2, 0.0, result);
}

int main(int, char**)
{
    cv::Mat orgframe;
    cv::Mat firstFrame, frame, frameGray, prevFrame, prevFrameGray, fgmask, fgmaskColor, result;
    cv::VideoCapture cap;

    cv::Mat blended;
    std::vector<cv::Mat> results;

    cap.open("domokun.mp4");
    if (!cap.isOpened()) return -1;

    cv::Size imageSize;
    cap >> orgframe;
    cv::resize(orgframe, frame, cv::Size(), 0.5, 0.5);
    imageSize = frame.size();

    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    cv::VideoWriter writer = cv::VideoWriter("./out_put.mp4", fourcc, 24.0, imageSize);

    cv::Mat background = cv::imread("background.jpeg");
    cv::resize(background, background, frame.size());

    char key = 0;
    while (key != 'q') {

        cap >> orgframe;
        if (orgframe.empty()) break;
        cv::resize(orgframe, frame, cv::Size(), 0.5, 0.5);
        imageSize = frame.size();

        if (firstFrame.empty()) {
            firstFrame = frame.clone();
        }
        cv::absdiff(frame, firstFrame, fgmask);
        cv::cvtColor(fgmask, fgmask, cv::COLOR_BGR2GRAY);
        cv::threshold(fgmask, fgmask, 30, 255, cv::THRESH_BINARY);

        cv::cvtColor(fgmask, fgmaskColor, cv::COLOR_GRAY2BGR);
        cv::bitwise_and(frame, fgmaskColor, result);

        cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
        if (!prevFrameGray.empty()) {
            cv::Mat flow;
            cv::calcOpticalFlowFarneback(prevFrameGray, frameGray, flow, .4, 1, 12, 2, 8, 1.2, 0);
            drawOpticalFlow(result, flow);
        }

        if ((int)(cap.get(cv::CAP_PROP_POS_FRAMES)) % (int)std::round(cap.get(cv::CAP_PROP_FPS) * 0.5) == 0) {

            results.push_back(result.clone());
            if (results.size() >= 5) {
                results.erase(results.begin());
            }

        }
        int last = (int)results.size() - 1;
        blended = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
        for (int i = last; i > 0; i--) {
            if (results[i].empty()) continue;
            if (i == last) {
                results[i].copyTo(blended);
            } else {
                cv::addWeighted(blended, 0.8, results[i], 0.2, 0.0, blended);
            }
        }
        if (!prevFrameGray.empty()) {
            cv::Mat flow;
            cv::calcOpticalFlowFarneback(prevFrameGray, frameGray, flow, .4, 1, 12, 2, 8, 1.2, 0);
            drawOpticalFlow(blended, flow);
        }

        cv::addWeighted(blended, 1.0, background, 0.5, 0, blended);

        frame.copyTo(blended, fgmask);

        cv::imshow("frame", frame);
        cv::imshow("mask", fgmask);
        cv::imshow("result", result);

        if(!blended.empty()){
            cv::imshow("blended", blended);
        }

        prevFrameGray = frameGray.clone();
        prevFrame = frame.clone();

        if(!blended.empty()){
            writer << blended;
        }

        key = cv::waitKey(1);

    }

    writer.release();
    cap.release();

    return 0;
}