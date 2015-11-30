/**
 * @file generate_vocabulary.cpp
 * @brief 
 *     随机从所有草图里采样若干特征，对特征进行聚类，聚类中心组成视觉词典
 *
 * @author An Boqing , hunkann@gmail.com
 * @version 1.0
 * @date 2015-11-29
 *
 * Copyright (c) 2015 An Boqing
 * All rights reserved.
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *                                        
 *      http://www.apache.org/licenses/LICENSE-2.0 
 *                                        
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "config.h" // PathConfig FeatureConfig
#include "file_tool.h" // get sketch path list
#include "galif.h" // Galif compute feature 
#include "typedefs.h"

#include <opencv2/opencv.hpp>
#include <glog/logging.h>

#include <iostream>
#include <vector>
#include <sstream>

using namespace sbe;

int main(int argc, char**argv){

    if(argc<2){
        std::cout << "Usage : ./" << std::string(argv[0] );
        std::cout << "  path/to/path.conf";
        std::cout<< std::endl; 
    }

    std::vector<std::string> sketch_path_vec; // 存储所有草图的路径

    std::shared_ptr<PathConfig> pconf = PathConfig::GetInstance();
    pconf->load(argv[1]);
    std::string& sketch_path = pconf->_conf_map["sketch_path"];
    std::string& feature_conf_path = pconf->_conf_map["feature_conf_path"];
    std::string& feature_data_path = pconf->_conf_map["feature_data_path"];

    // 先获取所有草图的文件名
    if(GetFileListInPath(sketch_path,sketch_path_vec)){
        LOG(FATAL)<< " get sketch file list failed !";
    }

    std::shared_ptr<FeatureConfig> fconf = FeatureConfig::GetInstance();
    fconf->load(feature_conf_path);

    Galif feature_extractor(*fconf);

    // 把每个草图的特征提取出来，并保存成特征文件(什么格式)
    for(auto path : sketch_path_vec){
         // 从path中读取出sketch
        cv::Mat img = cv::imread(path.c_str());
        Features_t features;
        KeyPoints_t keypoints;
        //抽取出当前图片的特征
        feature_extractor.compute(img,keypoints,features);
        //存一个相应的特征文件:file_tool里面写一个函数
        WriteVec<float>(feature_data_path,path,features);
    }

    return 0;
}
