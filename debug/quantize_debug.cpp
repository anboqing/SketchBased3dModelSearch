#include "../include/quantize/quantize.h"
#include <iostream>
#include <glog/logging.h>
#include <cassert>
#include <cmath>
#include <ios>

using namespace std;

bool TestCalcLength(){
    std::vector<float> vec = {1,1};
    cout << CalcLength(vec) << endl;

    return true;
}

bool TestCalcInnerProduct(){
    std::vector<float> v1 = {1,1};
    std::vector<float> v2 = {0,0};
    std::vector<float> v3 = {1,1};
    float r1 = CalcInnerProduct(v1,v2);
    float r2 = CalcInnerProduct(v1,v3);
    cout << r1 << endl;
    cout << r2 << endl;
    if(r1!=0 || r2 !=2)return false;
    return true;
}

bool TestSimi(){
   vector<float> v1 = {1,1};
   vector<float> v2 = {1,1};
   vector<float> v3 = {0,1};
   float r1 = CalcSimilarity(v1,v2);
   float r2 = CalcSimilarity(v2,v3);
   std::cout.setf(std::ios::hex,std::ios::basefield);
   cout << "r1: "<< r1 << endl;
   //cout << "r2: "<< r2 << endl;
   cout << "r1 !=1.0f "<< (r1!=static_cast<float>(1)) << endl;
   //cout << (r2 != (float)(1.0/sqrt(2))) << endl;
   if(r1!=(float)1.0f )return false;
   //if( r2 != (float)(1.0/sqrt(2)))return false;
   return true;
}

int main(){
   //assert(TestCalcLength());
    //assert(TestCalcInnerProduct());
    assert(TestSimi());
    return 0;
}
