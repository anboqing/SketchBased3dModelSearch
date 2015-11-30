#include <opencv2/opencv.hpp>
#include <complex>
#include <cstdio>
#include <fstream>
#include <iostream>

#include "typedefs.h"
#include "galif.h"
#include "feature.h"
#include "detector.h"
#include "config.h"

#include <memory>

#include <iostream>
using namespace std;
using namespace sbe;

int main(int argc, char ** argv){


    if(argc<3){
        cout << "Usage : `$0` image　keypoints features "<<endl; 
        return -1;
    }
    std::shared_ptr<FeatureConfig> config = FeatureConfig::GetInstance();
    config->load("../config/feature.xml");
    //Galif g(256,4,4,0.12,0.02,0.3,0.1,true,"none","grid",625);
    Galif g(*config);

    KeyPoints_t keypoints;
    Features_t features;
    cv::Mat image = cv::imread(argv[1]);

    //g.OutputFilters();

    g.compute(image,keypoints,features);

    // 把keypoints 和features都输出
    ofstream kpoints_ofs(argv[2]);
    kpoints_ofs << keypoints.size() << " " << keypoints[0].size() << endl;
    for(uint i = 0; i<keypoints.size(); i++){
        for(uint j = 0;j<keypoints[i].size(); ++j){
            kpoints_ofs << keypoints[i][j] << " " ; 
        } 
        kpoints_ofs << endl;
    }
    kpoints_ofs.close();


    ofstream ft_ofs(argv[3]);
    ft_ofs << features.size() << features[0].size() << endl;
    for(uint i= 0 ;i<features.size() ; ++i){
        for(uint j = 0 ;j<features[i].size() ; ++j){
            ft_ofs << features[i][j] << " " ; 
        } 
        ft_ofs << endl;
    }
    ft_ofs.close();

    cout << " compelete ! " << endl;

    return 0;
}
