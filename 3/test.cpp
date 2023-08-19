#include <iostream>
#include <vector>
#include "Stash.h"

using namespace std;

int main(int argc, const char* argv[]){

    std::string imagePath = "./photo.jpg";

    cv::Mat sourceImage = cv::imread(imagePath, cv::IMREAD_COLOR);
    if(sourceImage.data == 0){
        printf("File not found\n");
        exit(0);
    }

    cv::Mat grayImage;
    cv::cvtColor(sourceImage, grayImage, cv::COLOR_BGR2GRAY);


    cout << "min: " << Stats::min(grayImage) << endl;
    cout << "max: " << Stats::max(grayImage) << endl;
    cout << "average: " << Stats::average(grayImage) << endl;
    cout << "median: " << Stats::median(grayImage) << endl;
    cout << "mode: ";
    vector<int> mode_data = Stats::mode(grayImage);
    for (int i = 0; i < mode_data.size(); i++) {
        cout << mode_data[i] <<  " ";
    }
    cout << endl;
    cout << "variance: " << Stats::variance(grayImage) << endl;
    cout << "stdev: " << Stats::stdev(grayImage) << endl;

    int width = sourceImage.cols;
    int height = sourceImage.rows;

    cv::Mat hist = histogram::make(grayImage, cv::Size(width, height));

    histogram::show("histgram", hist);
    cv::waitKey(0);

    return 0;
}