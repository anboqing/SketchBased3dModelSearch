#include "read_data.h"
#include <stdlib.h>
using namespace std;
using namespace cv;

int main(){
    
    cv::Mat dataMat; 

    LoadData("testSet.txt",dataMat,0,2,1);

    cv::Mat centers,labels;

    const unsigned int MAX_ITER = 1000;
    const double accuracy_threashold = 0.1;

    cv::kmeans(dataMat,
            4,labels,
            TermCriteria(
                TermCriteria::EPS+TermCriteria::COUNT,
                MAX_ITER,
                accuracy_threashold),
            10,// attempts : flag to specify the number of times the algorithm is executed using different initial labellings.
            cv::KMEANS_PP_CENTERS,
            centers);
    for(int i = 0 ;i<centers.rows;++i){
        for(int j = 0; j<centers.cols ; ++j){
            cout << centers.at<float>(i,j) << " "; 
        } 
        cout << endl;
    }
    return 0;
}
