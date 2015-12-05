/**
 * @file generate_index.cpp
 * @brief 生成倒排索引
 * @author An Boqing , hunkann@gmail.com
 * @version 1.0.0
 * @date 2015-12-04
 */

/* Copyright (c) 2015 An Boqing
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
#include "config.h"
#include <vector>
#include <map>
#include <sstream> // ostringstream

#include <unordered_map> // 倒排索引结构

#include <iostream>
#include <glog/logging.h>

using namespace std;


int main(int argc,char** argv){

    if(argc < 2){
        cout << "Usage : ./generate_index path/to/path.conf" << endl;
        return -1;
    }

    google::InitGoogleLogging(argv[0]);
    google::LogToStderr();

    typedef std::size_t WordIndex,WordCount;
    typedef std::map<std::size_t,std::size_t> Hist_t;

    std::vector<std::vector<float> > vocabulary; // 单词表
    vector<Hist_t> all_hist; // 所有直方图
    std::map<WordIndex,WordCount> term_frequency_counter; //用来统计一个单词在所有文档中频率的数据结构,需要保存起来，因为查询时候量化query的时候要用到
    std::vector<WordCount> hist_word_counter; // 每个直方图中单词总数
    // 存储每个sketch(map)中每个单词（下标）以及对应的tf-idf值
    std::vector<std::map<std::size_t,float> > tf_idf_map_vec;  // 这个也保存，查询的时候用


    // 准备配置文件
    std::shared_ptr<PathConfig> pconf = PathConfig::GetInstance();
    pconf->load(argv[1]);

    const string& vocabulary_path = pconf->_conf_map["vocabulary_path"]; 
    const string& hist_path = pconf->_conf_map["hist_path"];

    string& index_root_path = pconf->_conf_map["index_root_path"];

    LOG(INFO) << " load vocabulary .. ";
    // 读取单词表
    LoadData2stdVec(vocabulary_path,vocabulary);
    LOG(INFO) << "vocabulary size : "<<vocabulary.size();
    
    // 读取直方图
    LOG(INFO) << " load histogram ... ";
    LoadHist<std::size_t,std::size_t>(all_hist,hist_path);

    std::size_t N = all_hist.size(); // 所有sketch的数量 , 也要保存起来，量化用户查询的时候用

    DLOG(INFO) << "N : "<< all_hist.size();
    
    // 读取所有特征文件的文件名到内存,用来提取元数据（特征对应的模型名)
    std::vector<std::string> feature_names;
    const string& feature_path = pconf->_conf_map["feature_data_path"];

    LOG(INFO) << "statistics each word's term_frequency ... ";
    // 遍历所有直方图，统计每个单词在所有文档出现的次数
    for(vector<Hist_t>::size_type idx = 0 ;idx < all_hist.size() ;++idx){
         // 遍历每个直方图，把每个单词的频率汇总到全局单词频率中
         // 把每个直方图中所有单词的数量
         std::size_t hist_wnum = 0; // 统计当前sketch中单词的总数
         for(auto pair : all_hist[idx]){
            // pair .first 是单词在单词表里的下标
            // pair.second 是单词在当前sketch里出现的次数
            term_frequency_counter[pair.first]+=pair.second;
            hist_wnum+=pair.second;
         }
         hist_word_counter.push_back(hist_wnum);
    }
    DLOG(INFO)<<" hist_word_counter.size() : " << hist_word_counter.size();

    // 保存 每个单词在所有文档中出现的次数
    const string& tfc_path = pconf->_conf_map["tfc_path"];
    SaveTFC(tfc_path,term_frequency_counter);

    LOG(INFO) << " calculate tf-idf ... ";
    
    // 修改直方图中h_j 为 (h_j/\sum h_j)*log(N/tf_j);
    for(vector<Hist_t>::size_type idx = 0; idx < all_hist.size(); ++idx){
        std::map<std::size_t,float> tf_idf_map; //存储当前hist中每个单词以及对应的tf-idf值
         float sum_hj = static_cast<float>(hist_word_counter[idx]);
         for(Hist_t::iterator iter = all_hist[idx].begin();iter!=all_hist[idx].end();++iter){         
            // pair .first 是单词在单词表里的下标
            // pair.second 是单词在当前sketch里出现的次数
            float tf_j = static_cast<float>(term_frequency_counter[iter->first]);
            float h_j = static_cast<float>(iter->second);
            // 修改 h_j
            float tf_idf = (h_j/sum_hj)*log(static_cast<float>(N)/tf_j);
            tf_idf_map.insert(make_pair(iter->first,tf_idf)); 
         }
         tf_idf_map_vec.push_back(tf_idf_map);
    }

    DLOG(INFO)<<"tf_idf_map_vec size : " << tf_idf_map_vec.size();

    // 保存修改后的hist
    const string& hist_path_tfidf = pconf->_conf_map["hist_path_tfidf"];
    SaveHist<std::size_t,float>(tf_idf_map_vec,hist_path_tfidf);


    typedef std::size_t VocabularyIndex,DocIndex;
    typedef std::map<DocIndex,float> TFIDF_T; 
    // 倒排索引数据结构
    std::map<VocabularyIndex,TFIDF_T> inverse_index;
    
    // 建立倒排索引
    LOG(INFO) << " generate inverse index .. " ;
   
    for(vector<map<std::size_t,float> >::size_type doc_id = 0; doc_id < tf_idf_map_vec.size() ;++doc_id){
         map<std::size_t,float>& hist = tf_idf_map_vec[doc_id]; // <voc_index,tf-idf>
         for(auto pair : hist){
             VocabularyIndex word_idx = pair.first; 
             float tf_idf = pair.second;
             auto& inverse_list = inverse_index[word_idx];
             inverse_list[doc_id]=tf_idf;
         }
    }

    DLOG(INFO) <<"inverse_index.size : "<< inverse_index.size();

    LOG(INFO) << " saving inverse index ... " ;

    std::size_t index_size = inverse_index.size();
    std::ostringstream o_index_name;
    o_index_name << "index_"<<index_size<<".dat";
    string index_name = o_index_name.str();
    index_root_path+=index_name;

    SaveIndex(index_root_path,inverse_index);

    LOG(INFO) << " done ! saving to : "<<index_root_path<<endl;
    return 0;
}
