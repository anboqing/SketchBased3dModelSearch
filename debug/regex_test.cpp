#include <regex>
#include <string>

#include <iostream>

using namespace std;
bool GetSizeByName(const std::string& name,unsigned* row,unsigned *col){
    // 正则表达式提取
    std::smatch results;
    std::regex pattern("(.*)(_)(.*?)(_)(.*?)(_)(.*?)\\.dat"); 
    if(std::regex_match(name,results,pattern)){
        *row = atoi((results[5]).str().c_str());   
        *col = atoi((results[7]).str().c_str());   
        return true;
    }else{
        return false;
    }
}

int main(){
    std::smatch result;
    std::string filename("D00001_1_218_64.dat");
    std::regex pattern("(.*)(_)(.*?)(_)(.*?)(_)(.*?)\\.dat");
    //std::regex pattern("D(.*)_*");
    bool valid = std::regex_match(filename,result,pattern);
    cout << "valid " << valid << endl;
    for(unsigned i = 0; i<result.size() ; ++i){
        cout << result[i] << endl; 
    }

    cout << result.str()<<endl;;
    cout << result[3].str() << endl;
    cout << result[5].str() << endl;
    cout << result[7].str() << endl;

    unsigned row,col;
    GetSizeByName(filename,&row,&col);
    cout << row << " " << col << endl;

    return 0;
}
