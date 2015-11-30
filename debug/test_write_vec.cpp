#include "file_tool.h"

#include <iostream>
#include <vector>

using namespace std;


int main(){
    vector<vector<float> > vec = {{1,2,3},{3,2,2}};
    WriteVec<float>("./","./feature.model",vec);
    return 0 ;
}


/*
 * g++ test_write_vec.cpp -lboost_system -lboost_filesystem -I../include/ -std=c++11 `pkg-config --libs --cflags opencv` -lglog -o file_tool_debug
 */
