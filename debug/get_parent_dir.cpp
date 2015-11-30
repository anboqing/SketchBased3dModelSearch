#include <boost/filesystem.hpp>

#include <iostream>

using namespace std;
using namespace boost::filesystem;

int main(){
    path p("/home/anboqing/Code/sbe/data/test_sketch/D00001/71.jpg");
    cout << p.parent_path() << endl;

    string parent_path = p.parent_path().string();

    string::size_type pos = parent_path.rfind("/");
    
    string parent_dir = parent_path.substr(pos+1,parent_path.length());

    cout << parent_dir << endl;

    return 0;
}
