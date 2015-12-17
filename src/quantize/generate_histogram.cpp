/**
 * @file generate_histogram.cpp
 * @brief :
 *  Represent each view as histogram of visual word frequency. quantize all local  \
 *  features x_i from a given sketch against the visual vocabulary,representing them 
 *  as the index_{q_ij} of their closest visual word:
 *      q_ij = arg min{x_i - c_j}
 *
 *  we now define the entries h_j of the final histogram of visual word representation 
 *  h that encodes a view as:
 *          h_j = |{q_ij}|.
 *  each dimension j in the feature vector corresponds to a visual word and encodes 
 *  the number of those words appearing in a sketch.
 *
 * @author An Boqing , hunkann@gmail.com
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
 * 
 */
#include "file_tool.h"
#include "quantize.h"
#include "config.h"
#include <glog/logging.h>

#include <memory>
#include <iostream>
#include <vector>
#include <map>
#include <queue>

#include <boost/filesystem.hpp>

using namespace std;

typedef std::map<std::size_t,std::size_t> Hist_t; // 直方图

int main(int argc, char** argv){

    if(argc < 2){
        cout << "Usage : ./generate_histogram path/to/path.conf" << endl;
        return -1;
    }

    google::InitGoogleLogging(argv[0]);

    std::vector<std::vector<float> > vocabulary; // 单词表
    vector<Hist_t> all_hist; // 所有直方图 (由于内存不足装下全部hist，所以分批读)

    // 准备配置文件
    std::shared_ptr<PathConfig> pconf = PathConfig::GetInstance();
    pconf->load(argv[1]);

    FLAGS_log_dir = pconf->_conf_map["log_dir"];

    const string& vocabulary_path = pconf->_conf_map["vocabulary_path"]; 
    const string& hist_path = pconf->_conf_map["hist_path"];
    // 读取单词表
    LoadData2stdVec(vocabulary_path,vocabulary);

    DLOG(INFO) <<"单词表大小 "<< vocabulary.size();
        
    DLOG(INFO)<< "单词维度 " << vocabulary[0].size();

    // 读取所有特征文件的文件名到内存
    std::vector<std::string> feature_names;
    const string& feature_path = pconf->_conf_map["feature_data_path"];

    GetFileListInPath(feature_path,feature_names);

    DLOG(INFO) << "所有sketch数量 " << feature_names.size() ;

    // 对每个特征文件
    size_t total = feature_names.size();
    size_t count = 1;
    for(auto feature_name : feature_names){
        std::cout << " quantize .. " << count++ <<"/"<< total<< "\r"<< std::flush;
        // 读取特征文件
        vector<vector<float> > features;
        LoadData2stdVec(feature_name,features);
        // 每个feature 和单词表里所有 单词计算相似度 存入优先级队列
        Hist_t hist = QuantizeFeature(features,vocabulary);

        /*
        boost::filesystem::path p(feature_name);

        boost::filesystem::path final_path = p.parent_path()/(p.stem().string()+".hist");

        std::cout << final_path.string() << "\r"<<std::flush;

        SaveHist<std::size_t,std::size_t>(hist,final_path.string());
        */

        all_hist.push_back(hist);
    }
    std::cout << " quantize .. " << count <<"/"<<"\r"<< total<<std::endl;

    SaveHist<std::size_t,std::size_t>(all_hist,hist_path);

    return 0;
}
