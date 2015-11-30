#include <Eigen/Dense>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Eigen;

int main(){
    ifstream infile("../kmeans/testSet.txt"); 
    string line;
    Matrix<float,2,Dynamic> dataMat;
    while(getline(infile,line)){
        istringstream iss(line);
        Matrix<float,1,2> row;
        float data;
        while(iss >> data){
             row<<data;       
        }
        dataMat << row;
    }
    infile.close();
    return 0;
}
