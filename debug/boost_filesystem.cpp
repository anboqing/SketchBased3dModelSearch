#include <boost/filesystem.hpp>
#include <iostream>

using namespace std;
using boost::filesystem::path;


int main(){
    
    path root("/home/anboqing/Code/sbe/data/features/");
    path p("feature_0_1.dat");
    root/=p;
    cout << root.string() << endl;

    p+="hahah";
    cout << p.string() << endl;

    return 0;
}

