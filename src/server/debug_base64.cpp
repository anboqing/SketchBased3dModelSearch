#include <algorithm>
#include <iostream>  
#include <sstream>  
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "base64.hpp"
using namespace std;  

int main()  
{  
    uchar data[b.size()];
    istringstream iss(b);
    for(size_t i = 0 ; i<b.size();++i){
        uint32_t n;
        iss>>n;
        data[i] = n;
    }

    cout << b.size() << endl;
    
    cv::Mat img = cv::Mat(100,100,CV_8UC4,data).clone();
    cv::cvtColor(img,img,
    cv::imshow("hello",img);
    cv::waitKey(0);
    return 0;  
}  
