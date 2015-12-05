/**
 * @file query.cpp
 * @brief 用户给出一个输入，返回查询结果
 * @author An Boqing , hunkann@gmail.com
 * @version 1.0.0
 * @date 2015-12-05
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
#include <opencv2/opencv.hpp>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>
#include <set>

#include "config.h"
#include "file_tool.h"
#include "typedefs.h"
#include "galif.h"
#include "quantize.h"

using namespace std;
using namespace sbe;


int main(int argc,char** argv){

    if(argc < 3){
        std::cout << "usage : ./query path/to/path.conf path/to/query.jpg"<< std::endl; 
    }
    // 读取所有路径
    std::shared_ptr<PathConfig> pconf = PathConfig::GetInstance(); 
    pconf->load(argv[1]);

    /***********************************************************/
    InverseIndex_t inverse_index; // 倒排索引
    vector<map<size_t,float> > hist_tfidf; // 文档向量表
    map<size_t,size_t> term_frequency_map; // 单词的文档频率 word_index,word_count
    vector<vector<float> > vocabulary; // 单词表
    shared_ptr<sbe::Galif> p_galif; //特征提取器
    set<size_t> candidate_doc_set; // 包含查询词的文档index集合
    //vector<Hist_t> candidate_hists; // 包含查询词的文档hist集合
    /***********************************************************/
    // 获取草图文件列表
    vector<string> sketch_filename_list;
    const string& sketch_path = pconf->_conf_map["sketch_path"];
    GetFileListInPath(sketch_path,sketch_filename_list);
    

    
    LOG(INFO) << " loading data .. " ;

    shared_ptr<FeatureConfig> fconf = FeatureConfig::GetInstance();
    const std::string& feature_conf_path = pconf->_conf_map["feature_conf_path"];
    fconf->load(feature_conf_path);
    p_galif = make_shared<sbe::Galif>(*(new sbe::Galif(*fconf))); 

// 读取倒排索引
    const std::string& index_path = pconf->_conf_map["index_path"];
    LoadIndex(index_path,inverse_index);
// 读取每篇文档的直方图
    const std::string& hist_path_tfidf = pconf->_conf_map["hist_path_tfidf"];
    LoadHist<std::size_t,float>(hist_tfidf,hist_path_tfidf);
// 读取每个单词的term_frequency
    const std::string& tfc_path = pconf->_conf_map["tfc_path"];
    LoadTFC(tfc_path,term_frequency_map);
// 读取单词表
    const string& vocabulary_path = pconf->_conf_map["vocabulary_path"];
    LoadData2stdVec(vocabulary_path,vocabulary);
// 读取查询草图
    cv::Mat query_img = cv::imread(argv[2]);    
// 提取查询草图特征
    sbe::KeyPoints_t keypoints;
    sbe::Features_t features; //查询草图的特征
    
    LOG(INFO) << " computing query feature .. " ;

    p_galif->compute(query_img,keypoints,features);
// quantize查询草图生成查询histogram
    LOG(INFO) << " quantizing query feature ... " ;
    Hist_t query_hist = QuantizeFeature(features,vocabulary);     
// -在倒排索引中提取包含查询词的所有文档集合
    LOG(INFO) << " retrieve candidate doc set .. " ;
    size_t sum_wordcount=0; // 查询草图的单词数量
    for(auto& wordid_wc : query_hist){
        // 取出查询特征里的每个单词
        size_t word_id = wordid_wc.first; 
        sum_wordcount+=wordid_wc.second;
        // 在倒排索引中查找当前单词的倒排列表
        map<size_t,float>& inverse_list = inverse_index[word_id];
        CHECK(inverse_list.size()!=0);
        // 遍历当前倒排列表，把其中的所有文档id拿出来存入候选doc集合
        for(auto& docid_weigth : inverse_list){
            candidate_doc_set.insert(docid_weigth.first);
        }
    }
    LOG(INFO) << " generate query feature vector ...";
// -生成查询特征向量 hj=(hj/sum_hj)log(N/fj)
    map<size_t,float> query_feature_vector;
    size_t N = hist_tfidf.size();// 总的文档数
    for(auto& wordid_wc : query_hist){
        // 提取当前单词在所有文档出现的次数 
        size_t term_frequency = term_frequency_map[wordid_wc.first];
        size_t word_count = wordid_wc.second;
        float tfidf = (word_count/sum_wordcount)*log(N/term_frequency);
        query_feature_vector.insert(std::make_pair(wordid_wc.first,tfidf));
    }
// --把候选文档集合的histogram和查询特征向量求相似度，排序，返回结果
    LOG(INFO) << " calculate similarity with each candidate doc.. ";
    priority_queue<Compair_t,std::vector<Compair_t>,Comp> sort_pq;

    CHECK(candidate_doc_set.size()!=0);

    for(auto doc_id : candidate_doc_set){
         // 获取一个候选文档特征向量
         map<size_t,float>& candidate_feature_vector = hist_tfidf[doc_id];
         // 和查询特征向量求相似度
         float simi = CalcMapSimilarity(query_feature_vector,candidate_feature_vector);
         
         // 把docid,simi存入优先级队列进行排序
         sort_pq.push(make_pair(doc_id,simi));
    }

    // 返回最相似的文档
    size_t target_docid = sort_pq.top().first;

    LOG(INFO) << " show the most similar sketch " ;
    // 读取最相似的草图
        cv::Mat target_img = cv::imread(sketch_filename_list[target_docid]);

        cv::imshow("target_img",target_img);
    
        cv::waitKey(0);

    return 0;
}
