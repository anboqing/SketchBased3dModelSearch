#include "../include/quantize/quantize.h"
#include <map>
#include <iostream>

using namespace std;

int main(){
    map<size_t,float> l,r;
    l.insert(make_pair(1,0.1));
    l.insert(make_pair(2,0.2));
    l.insert(make_pair(4,0.4));
    r.insert(make_pair(1,0.1));
    r.insert(make_pair(2,0.2));
    r.insert(make_pair(3,0.3));
    
    cout << CalcMapLength(l) << endl;

    cout << CalcMapInnerProduct(l,r) << endl;

    cout << CalcMapSimilarity(l,r) << endl;

    return 0;
}
