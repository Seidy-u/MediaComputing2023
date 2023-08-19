//
//  Stats.h
//  
//
//  Created by KITASAKATAKAYUKI on 2022/04/27.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <cassert>
#include <opencv2/opencv.hpp>

#ifndef Stats_h
#define Stats_h

using namespace std;

namespace histogram {

#define DEPTH (256)

cv::Mat make(const cv::Mat src, const cv::Size hsize)
{
	cv::Mat histImage;
	
	if(src.channels() != 1){
		std::cout << "入力画像はグレースケールのみです" << std::endl;
		return histImage;
	}
		
	std::vector<int> histArray(hsize.width);  //ヒストグラム配列
	
	//ヒストグラムの計算
	unsigned char v;
	for(int j=0; j<src.rows; j++){
		for(int i=0; i<src.cols; i++){
			//画素値の頻度計測
			v = src.at<unsigned char>(j, i);
			histArray[v]++;
		}
	}

	histImage = cv::Mat(hsize, CV_8UC3);

	//最大頻度を求める
	int max = 0;
	for(int i=0; i<hsize.width; i++){
		if(max < histArray[i]){
			max = histArray[i];
		}
	}
	
	cv::Point s, e;
	for(int i=0; i<hsize.width; i++){
		s.x = i;
		s.y = hsize.height - 0;
		e.x = i;
		e.y = hsize.height - (int)(histArray[i]*hsize.height/max);
		cv::line(histImage, s, e, CV_RGB(255,255,255));
	}
	
	return histImage;
}

void show(const std::string winname, const cv::Mat hImage)
{
	cv::imshow(winname, hImage);
}

}

namespace Stats {  //統計量計測関係の名前空間

#define WHITE (255)
#define BLACK (0)

int frequency(cv::Mat hist, int x)
{
	int low = -1;
	int high = hist.rows;

	while (high - low > 1) {
		int mid = low + (high - low) / 2;
		if (static_cast<int>(hist.at<unsigned char>(mid, x)) == BLACK) low = mid;
		else high = mid;
	}

	return hist.rows - high;
}

int min(cv::Mat src, cv::Point upperLeft = cv::Point(0, 0), cv::Point lowerRight = cv::Point(-1, -1))
{
	assert(src.channels() == 1);

	if (lowerRight.x == -1 && lowerRight.y == -1) lowerRight = cv::Point(src.cols - 1, src.rows - 1);

	assert(0 <= lowerRight.x && lowerRight.x < src.cols);
	assert(0 <= upperLeft.x && upperLeft.y < src.rows);
	assert(upperLeft.x <= lowerRight.x && upperLeft.y <= lowerRight.y);

	int _min = src.at<unsigned char>(upperLeft.y, upperLeft.x);

	for(int i = upperLeft.y;i <= lowerRight.y;i++) {
		for(int j = upperLeft.x;j <= lowerRight.x;j++) {
			int v = static_cast<int>(src.at<unsigned char>(i, j));
			if (_min > v) _min = v;
		}
	}

	return _min;
}

int min(cv::Mat src, bool hist) {
	if (!hist) return min(src);

	cv::Mat binImage;
	if (src.channels() == 1) binImage = src;
	else {
	    cv::Mat grayImage;
    	cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);	
		cv::threshold(grayImage, binImage, 254, WHITE, cv::THRESH_BINARY);
	}

	int y = binImage.rows - 1;
	for (int x = 0;x < binImage.cols;x++) {
		if (static_cast<int>(binImage.at<unsigned char>(y, x)) == WHITE) return x;
	}

	return -1;
}

int max(cv::Mat src, cv::Point upperLeft = cv::Point(0, 0), cv::Point lowerRight = cv::Point(-1, -1))
{
	assert(src.channels() == 1);

	if (lowerRight.x == -1 && lowerRight.y == -1) lowerRight = cv::Point(src.cols - 1, src.rows - 1);

	assert(0 <= lowerRight.x && lowerRight.x < src.cols);
	assert(0 <= upperLeft.x && upperLeft.y < src.rows);
	assert(upperLeft.x <= lowerRight.x && upperLeft.y <= lowerRight.y);

	int _max = src.at<unsigned char>(upperLeft.y, upperLeft.x);

	for(int i = upperLeft.y;i <= lowerRight.y;i++) {
		for(int j = upperLeft.x;j <= lowerRight.x;j++) {
			int v = static_cast<int>(src.at<unsigned char>(i, j));
			if (_max < v) _max = v;
		}
	}

	return _max;
}

int max(cv::Mat src, bool hist) {
	if (!hist) return max(src);

	cv::Mat binImage;
	if (src.channels() == 1) binImage = src;
	else {
	    cv::Mat grayImage;
    	cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);	
		cv::threshold(grayImage, binImage, 254, WHITE, cv::THRESH_BINARY);
	}

	int y = binImage.rows - 1;
	for (int x = binImage.cols - 1;x >= 0;x--) {
		if (static_cast<int>(binImage.at<unsigned char>(y, x)) == WHITE) return x;
	}

	return -1;
}

double average(cv::Mat src, cv::Point upperLeft = cv::Point(0, 0), cv::Point lowerRight = cv::Point(-1, -1))
{
	assert(src.channels() == 1);
	
	if (lowerRight.x == -1 && lowerRight.y == -1) lowerRight = cv::Point(src.cols - 1, src.rows - 1);

	assert(0 <= lowerRight.x && lowerRight.x < src.cols);
	assert(0 <= upperLeft.x && upperLeft.y < src.rows);
	assert(upperLeft.x <= lowerRight.x && upperLeft.y <= lowerRight.y);

	double sum = 0.0;

	for(int i = upperLeft.y;i <= lowerRight.y;i++)
		for(int j = upperLeft.x;j <= lowerRight.x;j++)
			sum += static_cast<double>(src.at<unsigned char>(i, j));

	return sum / static_cast<double>((lowerRight.y - upperLeft.y + 1) * (lowerRight.x - upperLeft.x + 1));
}

double average(cv::Mat src, bool hist)
{
	if (!hist) return average(src);

    cv::Mat binImage;
	if (src.channels() == 1) binImage = src;
	else {
	    cv::Mat grayImage;
    	cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);	
		cv::threshold(grayImage, binImage, 254, WHITE, cv::THRESH_BINARY);
	}

	int sum = 0;
	int num = 0;
	for (int x = 0;x < binImage.cols;x++) {
		int f = frequency(binImage, x);
		sum += (f * x);
		num += (f);
	}

	return (double)sum / (double)num;
}

double median(cv::Mat src, cv::Point upperLeft = cv::Point(0, 0), cv::Point lowerRight = cv::Point(-1, -1))
{
	assert(src.channels() == 1);

	if (lowerRight.x == -1 && lowerRight.y == -1) lowerRight = cv::Point(src.cols - 1, src.rows - 1);

	assert(0 <= lowerRight.x && lowerRight.x < src.cols);
	assert(0 <= upperLeft.x && upperLeft.y < src.rows);
	assert(upperLeft.x <= lowerRight.x && upperLeft.y <= lowerRight.y);

	vector<double> data;

	for(int i = upperLeft.y;i <= lowerRight.y;i++)
		for(int j = upperLeft.x;j <= lowerRight.x;j++)
			data.push_back(static_cast<double>(src.at<unsigned char>(i, j)));
	
	sort(data.begin(), data.end());

	return data.size() % 2 ? data[data.size() / 2] : (data[data.size() / 2] + data[data.size() / 2 - 1]) / 2.0;
}

double median(cv::Mat src, bool hist)
{
	if (!hist) return median(src);

	cv::Mat binImage;
	if (src.channels() == 1) binImage = src;
	else {
	    cv::Mat grayImage;
    	cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);	
		cv::threshold(grayImage, binImage, 254, WHITE, cv::THRESH_BINARY);
	}

	vector<int> histArray;
	int num = 0;
	for (int x = 0;x < binImage.cols;x++) {
		int f = frequency(binImage, x);
		histArray.push_back(f);
		num += f;
	}

	if (num % 2) {
		int sum = 0;
		for (int i = 0;i < histArray.size();i++) {
			if (sum + histArray[i] >= num / 2) return i;
			sum += histArray[i];
		}
	} else {
		int sum = 0;
		for (int i = 0;i < histArray.size();i++) {
			if (sum + histArray[i] >= num / 2) return (double)i;
			else if (sum + histArray[i] + 1 == num) return (double)(i + i - 1) / 2.0; 
			sum += histArray[i];
		}
	}

	return -1;
}

int mode(cv::Mat src, cv::Point upperLeft = cv::Point(0, 0), cv::Point lowerRight = cv::Point(-1, -1))
{
	assert(src.channels() == 1);

	if (lowerRight.x == -1 && lowerRight.y == -1) lowerRight = cv::Point(src.cols - 1, src.rows - 1);

	assert(0 <= lowerRight.x && lowerRight.x < src.cols);
	assert(0 <= upperLeft.x && upperLeft.y < src.rows);
	assert(upperLeft.x <= lowerRight.x && upperLeft.y <= lowerRight.y);

	map<int, int> mp;
	int max_idx = 0;

	for(int i = upperLeft.y;i <= lowerRight.y;i++) {
		for(int j = upperLeft.x;j <= lowerRight.x;j++) {
			int v = static_cast<int>(src.at<unsigned char>(i, j));
			mp[v]++;
			if (mp[max_idx] < mp[v]) max_idx = v;
		}
	}

	return max_idx;
}

double mode(cv::Mat src, bool hist)
{
	if (!hist) return mode(src);

	cv::Mat binImage;
	if (src.channels() == 1) binImage = src;
	else {
	    cv::Mat grayImage;
    	cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);	
		cv::threshold(grayImage, binImage, 254, WHITE, cv::THRESH_BINARY);
	}

	vector<int> histArray;
	int _mode = 0;
	for (int x = 0;x < binImage.cols;x++) {
		int f = frequency(binImage, x);
		histArray.push_back(f);
		if (histArray[_mode] < histArray[histArray.size() - 1]) _mode = histArray.size() - 1;
	}

	return _mode;
}

double variance(cv::Mat src, cv::Point upperLeft = cv::Point(0, 0), cv::Point lowerRight = cv::Point(-1, -1))
{
	assert(src.channels() == 1);
	
	if (lowerRight.x == -1 && lowerRight.y == -1) lowerRight = cv::Point(src.cols - 1, src.rows - 1);

	assert(0 <= lowerRight.x && lowerRight.x < src.cols);
	assert(0 <= upperLeft.x && upperLeft.y < src.rows);
	assert(upperLeft.x <= lowerRight.x && upperLeft.y <= lowerRight.y);

	double avg = average(src, upperLeft, lowerRight);
	double mean_sqrt_diff = 0.0;
	
	for(int i = upperLeft.y;i <= lowerRight.y;i++)
		for(int j = upperLeft.x;j <= lowerRight.x;j++)
			mean_sqrt_diff += pow(static_cast<double>(src.at<unsigned char>(i, j)) - avg, 2.0);

	return mean_sqrt_diff / static_cast<double>((lowerRight.y - upperLeft.y + 1) * (lowerRight.x - upperLeft.x + 1));
}

double variance(cv::Mat src, bool hist)
{
	if (!hist) return variance(src);

	cv::Mat binImage;
	if (src.channels() == 1) binImage = src;
	else {
	    cv::Mat grayImage;
    	cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);	
		cv::threshold(grayImage, binImage, 254, WHITE, cv::THRESH_BINARY);
	}

	double avg = average(binImage, true);
	double sum = 0.0;
	double num = 0.0;
	for (int x = 0;x < binImage.cols;x++) {
		int f = frequency(binImage, x);
		sum += (double)f * pow(x - avg, 2.0);
		num += (double)f;
	}

	return sum / num;
}

double stdev(cv::Mat src, cv::Point upperLeft = cv::Point(0, 0), cv::Point lowerRight = cv::Point(-1, -1))
{
	return sqrt(variance(src, upperLeft, lowerRight));
}

double stdev(cv::Mat src, bool hist)
{
	return sqrt(variance(src, hist));
}

}

#endif /* Stats_h */
