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
#include "typedefs.h"

#include <opencv2/opencv.hpp>
#include <glog/logging.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <random> // c++11
#include <algorithm> // shuffle
#include <fstream>

#include <regex> // c++11

#include <stdlib.h> // atoi

using namespace sbe;

/* ------------------------------------------------------------------------*/
/**
 * @brief 从特征文件名里面提取出来row 和col，eg. D00001_1_218_64.dat row=218,col=64
 *
 * @Param name : 文件名
 * @Param row : 特征矩阵的row number
 * @Param col : 特征矩阵的col number
 *
 * @Returns   : status
 */
/* ------------------------------------------------------------------------*/
bool GetSizeByName(const std::string& name,unsigned* row,unsigned *col){
    // 正则表达式提取
    std::smatch results;
    std::regex pattern("(.*)(_)(.*?)(_)(.*?)(_)(.*?)\\.dat"); 
    if(std::regex_match(name,results,pattern)){
       // results[0];   
       // results[2];   
        *row = atoi((results[5]).str().c_str());   
        *col = atoi((results[7]).str().c_str());   
        return true;
    }else{
        return false;
    }
}

int main(int argc, char**argv){

    if(argc<3){
        std::cout << "Usage : ./" << std::string(argv[0] );
        std::cout << "  path/to/path.conf";
        std::cout << " path/to/vocabulary.xml" ;
        std::cout<< std::endl; 
        return EXIT_FAILURE;
    }

    std::shared_ptr<VocabularyConfig> vconf = VocabularyConfig::GetInstance();
    vconf->load(argv[2]);

    const long int TOTAL_SAMPLE = vconf->_total_sample_num;// 总的采样数
    const long int CENTROIDS_NUM = vconf->_centroids_num; //　聚类中心数 
    const std::string vocabulary_data_path = vconf->_vocabulary_data_path;
    const int batch_num = vconf->_batch_num; // 配置文件
    const unsigned int MAX_ITER = vconf->_max_iter;
    const double accuracy_threashold = vconf->_accuracy_threshold;

    LOG(INFO) << TOTAL_SAMPLE;
    LOG(INFO) << CENTROIDS_NUM;
    LOG(INFO) << vocabulary_data_path;
    LOG(INFO) << batch_num;
    LOG(INFO) << MAX_ITER;
    LOG(INFO) << accuracy_threashold;

    std::vector<std::string> feature_path_vec; // 存储所有 特征文件名 的路径

    cv::Mat all_feature_m; // 存储所有特征,一行是一个特征

    std::shared_ptr<PathConfig> pconf = PathConfig::GetInstance();
    pconf->load(argv[1]);

    std::string& feature_data_path = pconf->_conf_map["feature_data_path"];

    // 把特征文件名列表读出来
    if(GetFileListInPath(feature_data_path,feature_path_vec)){
        LOG(FATAL)<< " get feature file list failed !";
    }
    
    long int sample_feature_count = 0;

    // 随机数生成器
    std::random_device  rd;      
    std::mt19937 gen(rd());

    bool stop = false;

    while(stop==false){
         //初始化这一批样本
         std::vector<std::string> batch_vec;
         batch_vec.resize(batch_num);

         // 均匀的选取 batch_num个文件名
         shuffle(feature_path_vec.begin(),feature_path_vec.end(),gen);
         std::copy_n(feature_path_vec.begin(),batch_num,batch_vec.begin());
         
         // 把batch中所有文件中的feature读进内存(cv::Mat）
            //从每个文件名中提取出row,col
         for(auto path : batch_vec){
            unsigned row,col;
            GetSizeByName(path,&row,&col);
            cv::Mat feature_mat;
            if(0!=LoadData2cvMat(path,feature_mat,row,col)){
                LOG(FATAL) << " load data to cv mat error ";
            }
            // 加入待聚类特征集合
            all_feature_m.push_back(feature_mat);
            sample_feature_count +=row;
            if(sample_feature_count>=TOTAL_SAMPLE){
                stop  = true;
                LOG(INFO) << "stop sample , total sample num : "<< sample_feature_count;
                break;
            }
         }
    }

    // 聚类出的聚类中心保存起来形成　vocabulary
    cv::Mat centers,labels;

    LOG(INFO) << " run kmeans ... ";
    
    cv::kmeans(all_feature_m,
            CENTROIDS_NUM,
            labels,
            cv::TermCriteria(
                cv::TermCriteria::EPS+cv::TermCriteria::COUNT,
                MAX_ITER,
                accuracy_threashold),
            10,// attempts : flag to specify the number of times the algorithm is executed using different initial labellings.
            cv::KMEANS_PP_CENTERS,
            centers);

    DLOG(INFO) << centers.size() << " " << centers.rows << " " << centers.cols;

    // 保存聚类中心
    WriteCVMat2File(vocabulary_data_path,centers);
        /*
    for(int i = 0 ;i<centers.rows;++i){
        for(int j = 0; j<centers.cols ; ++j){
            std::cout << centers.at<float>(i,j) << " "; 
        } 
        std::cout << std::endl;
    }

    std::cerr << centers.size() << std::endl;
    */

    /*
    for(int i = 0 ;i<labels.rows;++i){
        for(int j = 0; j<labels.cols ; ++j){
            std::cout << labels.at<int>(i,j) << std::endl; 
        } 
        std::cout << std::endl;
    }
    
    std::cerr << labels.size() << std::endl;
    */
    return 0;
}
