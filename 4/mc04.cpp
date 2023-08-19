//コンパイルコマンド　g++ mc04.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
int main (int argc, const char* argv[])
{
    //内蔵カメラからビデオキャプチャ“capture”生成
    cv::VideoCapture capture(0);  //“water.mov”にすればムービーファイルから映像を取り込む
    //キャプチャできたかチェック
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }
    //画像サイズの指定
    int width = 720, height = 405;
    //画像格納用インスタンス準備
    cv::Mat captureImage;  //オリジナルの入力画像格納用（大抵大きすぎる）
    cv::Mat frameImage(cv::Size(width, height), CV_8UC3);  //コンストラクタによる宣言時のインスタンス確保
    //cv::Mat frameImage = cv::Mat(cv::Size(width, height), CV_8UC3);  //こう書いても良い．宣言した後にインスタンス設定
    cv::Mat resultImage(cv::Size(width, height), CV_8UC3);
    cv::Mat result2Image(cv::Size(width, height), CV_8UC3);
    //ウィンドウの生成
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Result1");
    cv::moveWindow("Result1", frameImage.cols, 0);
    cv::namedWindow("Result2");
    cv::moveWindow("Result2", 0, frameImage.rows);

    // アニメ風
    cv::Mat result3Image(cv::Size(width, height), CV_8UC3);
    cv::namedWindow("Result3");
    cv::moveWindow("Result3", 0, frameImage.rows);

    //LUTの作成
    unsigned char LUT[256];
    for(int x=0; x<256; x++){
        unsigned char y = 2*x;
        if(x>127) y = 255;
        LUT[x] = y;
    }
    unsigned char ganmaLUT[256];
    double ganma = 2.0;
    for (int x = 0; x < 256; x++)
    {
        unsigned char y = (unsigned char)(255.0*pow((x/255.0), ganma));
        ganmaLUT[x] = y;
    }
    unsigned char animeLUT[256];
    int cnt = (sizeof(animeLUT) / sizeof(animeLUT[0]));
    int tone = 64;    // 階調
    for (int x = 0; x < cnt; x++)
    {
        animeLUT[x] = (float)(x / tone) / (float)( 255 / tone ) * 255.0f;
    }
    while(1){
        //カメラから1フレーム取り出し
        capture >> captureImage;
        //画像の大きさ変更
        cv::resize(captureImage, frameImage, frameImage.size());
        //トーンカーブでの返還処理
        for(int j=0; j<height; j++){
            for(int i=0; i<width; i++){
                cv::Vec3b s = frameImage.at<cv::Vec3b>(j,i);
                cv::Vec3b v = frameImage.at<cv::Vec3b>(j,i);
                cv::Vec3b g = frameImage.at<cv::Vec3b>(j,i);
                //s[0] = s[0]*2;
                //s[1] = s[1]*2;
                //s[3] = s[2]*2;
                v[0] = LUT[v[0]];
                v[1] = LUT[v[1]];
                v[2] = LUT[v[2]];
                result2Image.at<cv::Vec3b>(j,i) = v;
                s[0] = ganmaLUT[s[0]];
                s[1] = ganmaLUT[s[1]];
                s[2] = ganmaLUT[s[2]];
                resultImage.at<cv::Vec3b>(j,i) = s;
                g[0] = animeLUT[g[0]];
                g[1] = animeLUT[g[1]];
                g[2] = animeLUT[g[2]];
                result3Image.at<cv::Vec3b>(j,i) = g;

            }
        }
        //画像表示
        cv::imshow("Frame", frameImage);
        cv::imshow("Result1", resultImage);
        cv::imshow("Result2", result2Image);
        cv::imshow("Result3", result3Image);
        char key = cv::waitKey(30);
        if(key == 'q' || key == 'Q') break;
    }
    return 0;
}