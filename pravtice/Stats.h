//
//  Stats.h
//  
//
//  Created by KITASAKATAKAYUKI on 2022/04/27.
//

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#ifndef Stats_h
#define Stats_h

using namespace std;

namespace histogram {  //Stats名前空間

	cv::Mat make(const cv::Mat src, const cv::Size hsize)
	{
		cv::Mat histImage;
		
		if(src.channels() != 1){
			cout << "入力画像はグレースケールのみです" << endl;
			return histImage;
		}
			
		vector<int> histArray(hsize.width);  //ヒストグラム配列
		
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

	void show(const string winname, const cv::Mat hImage)
	{
		cv::imshow(winname, hImage);
	}

}

namespace Stats {  //統計量計測関係の名前空間

	//グレースケール
	int min(cv::Mat src)
	{
		int min = 10000000;
		unsigned char v;
		for(int j=0; j<src.rows; j++){
			for(int i=0; i<src.cols; i++){
				//画素値の頻度計測
				v = src.at<unsigned char>(j, i);
				if(min > v){
					min = v;
				}
			}
		}

		return min;
	}

	int max(cv::Mat src)
	{
		int max = -10000000;
		unsigned char v;
		for(int j=0; j<src.rows; j++){
			for(int i=0; i<src.cols; i++){
				//画素値の頻度計測
				v = src.at<unsigned char>(j, i);
				if(max < v){
					max = v;
				}
			}
		}

		return max;
	}

	double average(cv::Mat src)
	{
		double pixel_value_sum = 0;
		unsigned char v;
		for(int j=0; j<src.rows; j++){
			for(int i=0; i<src.cols; i++){
				//画素値の頻度計測
				v = src.at<unsigned char>(j, i);
				pixel_value_sum += v;
			}
		}

		return pixel_value_sum / (src.rows * src.cols);
	}

	double median(cv::Mat src)
	{
		vector<double> pixel_value_data;
		unsigned char v;
		for(int j=0; j<src.rows; j++){
			for(int i=0; i<src.cols; i++){
				//画素値の頻度計測
				v = src.at<unsigned char>(j, i);
				pixel_value_data.push_back(static_cast<int>(v));
			}
		}
		sort(pixel_value_data.begin(), pixel_value_data.end());

		unsigned int size = src.rows * src.cols;

		if (size % 1 == 0){
			return pixel_value_data[size / 2];
		}else{
			return static_cast<double>(pixel_value_data[size / 2] + pixel_value_data[(size / 2) - 1]) / 2;
		}
	}

	vector<int> mode(cv::Mat src)
	{
		unsigned char v;
		map<int, int> cnt;
		int maxCnt = 0;
		for(int j=0; j<src.rows; j++){
			for(int i=0; i<src.cols; i++){
				//画素値の頻度計測
				v = src.at<unsigned char>(j, i);
				int v_int = static_cast<int>(v);
				cnt[v_int] += 1;
				if (cnt[v_int] > maxCnt) {
					maxCnt = cnt[v_int];
				}
			}
		}
		vector<int> mode;

		for (auto const& [key, value] : cnt){
			if (value == maxCnt){
				mode.push_back(key);
			}
		}

		return mode;
	}

	double variance(cv::Mat src)
	{
		double ave = average(src);
		unsigned int size = src.rows * src.cols;
		unsigned char v;
		double tmp_data = 0;
		for(int j=0; j<src.rows; j++){
			for(int i=0; i<src.cols; i++){
				//画素値の頻度計測
				v = src.at<unsigned char>(j, i);
				int v_int = static_cast<int>(v);
				tmp_data += pow(v_int - ave, 2);
			}
		}

		return tmp_data / size;
	}

	double stdev(cv::Mat src)
	{
		return sqrt(variance(src));
	}

}

#endif /* Stats_h */
