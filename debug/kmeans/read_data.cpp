#include "read_data.h"
int main(){
    cv::Mat dataMat;
    LoadData("testSet.txt",dataMat,0,2,1);
    std::cout << dataMat.rows << " " << dataMat.cols << std::endl;
    return 0;
}

