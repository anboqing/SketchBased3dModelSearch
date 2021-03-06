#include "../include/file_tool.h"
#include "../include/config.h" // compile : link with ../src/config.cpp
/*
g++ filetool_debug.cpp ../src/config.cpp -I../include -lboost_system -lboost_filesystem -lglog `pkg-config --cflags --libs opencv` -std=c++11  -o filetool_debug
 */

#include <iostream>
#include <vector>


int main(int argc,char** argv){

    google::InitGoogleLogging(argv[0]);

    if(argc<2){
        return -1; 
    }

    using std::cout;
    using std::endl;

    /*
    std::shared_ptr<PathConfig> conf = PathConfig::GetInstance();
    conf->load("path.conf");

    auto& confmap = conf->_conf_map;
    std::string path = confmap["sketch_path"];
    std::vector<std::string> file_list;

    GetFileListInPath(path,file_list);

    
    for(auto name : file_list){
        cout << name << endl; 
    }
    */
    using namespace std;
    vector<vector<float> > data;
    Read2Vec(argv[1],data);
    
    cout << data.size() << " " << data[0].size() << endl;

    return 0;
}
