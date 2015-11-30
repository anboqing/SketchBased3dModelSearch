#include "../include/config.h"
#include <iostream>
using namespace std;

/*
 g++ config_debug.cpp ../src/config.cpp -I../include/ -lglog -std=c++11 -lboost_system -lboost_filesystem
 *
 */

int main(){
    /*
    std::shared_ptr<FeatureConfig> fc = FeatureConfig::GetInstance(); 
    fc->save("feature.test.conf");
    std::shared_ptr<SketchGenConfig> sc = SketchGenConfig::GetInstance();
    sc->save("sketch.conf");
    */
    
    std::shared_ptr<PathConfig> config = PathConfig::GetInstance();
    

    config->load("path.conf");

    config->save("path.conf");

    auto& conf = config->_conf_map;
    
    for(auto pair : conf){
        cout << pair.first << " - " << pair.second << endl; 
    }

    return 0;
}
