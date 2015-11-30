#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

int main(int argc,char** argv){
    using std::cout;
    using std::endl;
    if(argc<2){
        cout << "usage : ./this image_path" << endl; 
    }
    cv::Mat img = imread(argv[1]);

    cv::cvtColor(img,img,cv::COLOR_BGR2GRAY);
    // 用方框滤波滤波
    int feature_size = std::sqrt(img.size().area()*0.4);
    if(feature_size%4){
        feature_size += 4-(feature_size%4); 
    }
    int tile_size = feature_size/4;
    int kernal_size = tile_size;
    cout << kernal_size << endl;
    //cv::Mat filtered = cv::Mat::zeros(img.size(),img.type());
    Mat filtered;
    cv::boxFilter(img,filtered,CV_32F,cv::Size(kernal_size,kernal_size),cv::Point(-1,-1),false);
    cv::imshow("test",img);
    cv::waitKey(0);
    return 0;
}
