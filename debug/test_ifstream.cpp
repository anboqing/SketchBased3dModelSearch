#include <iostream>
#include <fstream>
using namespace std;

int main(){
    
    ifstream ifs("path.conf");

    if(ifs.is_open()){
        cout << " is open " << endl;
    }else{
        cout << " is not open " << endl;
    }

    cout << " 这是蛋疼的分割线-0--"<< endl;

    // reopen ifs
    ifs.open("path.conf");

    if(ifs.is_open()){
        cout << " open" << endl; 
    }else{
        cout << "not  open " << endl; 
    }

    ifstream ifss("path.conf");
    cout << ifss.is_open() << endl;

    return 0;
}
