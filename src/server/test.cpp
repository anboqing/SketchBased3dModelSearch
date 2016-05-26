
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <opencv2/opencv.hpp>
using namespace std;

int main(){ 
    string str("0 255 123 7 1");
    istringstream iss(str);
    char* buf = new char[str.size()];
    strcpy(buf,str.c_str());
    
    vector<uint8_t> data_vec;
    char *pch;
    pch = strtok(buf," ");
    data_vec.push_back(static_cast<uint8_t>(atoi(pch)));
    while(pch!=NULL){
        uint8_t n = static_cast<uint8_t>(atoi(pch));
        data_vec.push_back(n);
        pch = strtok(NULL," ");
    }

    // convert data_vec to uint8_t array
    uint8_t data[data_vec.size()];
    for(size_t i = 0; i< data_vec.size();++i){
        data[i] = data_vec[i];
    }
    cv::Mat img = cv::Mat(100,100,CV_8UC4,data).clone();
    cv::cvtColor(img,img,CV_RGB2GRAY);
    cv::imshow("h",img);
    cv::waitKey(0);
}
