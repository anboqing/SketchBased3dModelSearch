#include "../include/file_tool.h"

#include <vector>
#include <map>
#include <iostream>
#include <set>
using namespace std;

int main(){
    
    vector<map<int,int> > all_hist;

    LoadHist(all_hist,"/home/anboqing/Code/sbe/data/hist.dat");

    set<int> word_index;
    
    for( auto hist : all_hist){
        for(auto pair : hist){
            word_index.insert(pair.first); 
        }
    }


    for(auto index : word_index){
        cout << index << endl; 
    }

    return 0;
}
