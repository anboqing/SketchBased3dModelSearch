#include <boost/filesystem.hpp>
#include <iostream>

using namespace std;
using boost::filesystem::path;


int main(){
    
    /*
    path root("/home/anboqing/Code/sbe/data/features/");
    path p("feature_0_1.dat");
    root/=p;
    cout << root.string() << endl;

    p+="hahah";
    cout << p.string() << endl;
    */

    path p("/home/anboqing/test.png");

    cout << p.parent_path() << endl;
    string parent_path = p.parent_path().string(); 
    cout << parent_path.substr(parent_path.rfind("/")+1,parent_path.length()-1) << endl;;

    return 0;
}

