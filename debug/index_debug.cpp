#include "../include/file_tool.h"

#include <iostream>

using namespace std;

int main(){
    
    InverseIndex_t index;

    LoadIndex("/home/anboqing/Code/sbe/data/index_100.dat",
            index);

    cout << index.size() << endl;


    return 0;
}
