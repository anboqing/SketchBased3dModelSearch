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


#ifndef __QUANTIZE_H__
#define __QUANTIZE_H__

#include <map>
#include <queue>
#include <vector>
#include <glog/logging.h>
#include <math.h>

#include "typedefs.h"

using namespace std;

typedef unsigned int Index_t;
typedef std::pair<std::size_t,float> Compair_t;
typedef std::map<std::size_t,std::size_t> Hist_t;

class Comp{
    public:
        bool operator()(const Compair_t& lhs,const Compair_t& rhs ){
            if(lhs.second < rhs.second )return true;
            else return false;
        }
};

// 计算向量的模
float CalcLength(const vector<float>& vec){
    float sum  = 0.0;
    for(auto elem : vec ){
        sum += elem*elem;
    }
    return sqrt(sum);
}


// 计算特征向量的模
float CalcMapLength(std::map<size_t,float>& sparse_feature_vector){
    float sum = 0.0;
    for(auto elem : sparse_feature_vector){
        sum += elem.second * elem.second; 
    }
    return sqrt(sum);
}

float CalcInnerProduct(const vector<float>& lhs,const vector<float>& rhs){
    float sum  = 0.0;
    for(vector<float>::size_type ix = 0;ix < lhs.size() ;++ix){
        sum += lhs[ix]*rhs[ix]; 
    }
    return sum;
}

float CalcMapInnerProduct(std::map<size_t,float>& lhs,std::map<size_t,float>& rhs){
    float product = 0.0;
    for(auto& pair : lhs){
        // rhs.at(pair.first)
        product+= pair.second * rhs[pair.first]; 
    }
    return product;
}
/* ------------------------------------------------------------------------*/
/**
 * @brief 计算两个向量的相似度（余弦夹角）
 */
/* ------------------------------------------------------------------------*/
float CalcSimilarity(const vector<float>& lhs,const vector<float>& rhs){
    float dot,len;
    len = CalcLength(lhs)*CalcLength(rhs);
    dot = CalcInnerProduct(lhs,rhs);
    if(len==0){
        return 1; 
    }
    return dot/(len);
}

float CalcMapSimilarity(std::map<size_t,float>& lhs,std::map<size_t,float>& rhs){
    float dot,len;
    DLOG(INFO) << "before maplen ";
    len = CalcMapLength(lhs)*CalcMapLength(rhs);
    DLOG(INFO) << "end maplen ";
    DLOG(INFO) << "before mapprod ";
    dot = CalcMapInnerProduct(lhs,rhs);
    DLOG(INFO) << "end mapprod ";
    if(len==0){
        return 1; 
    }
    return dot/len;
}

/* ------------------------------------------------------------------------*/
/**
 * @brief 量化一个feature为视觉单词频率直方图
 *
 * @Param sketch_features 需要被量化sketch的所有feature
 * @Param vocabulary 用来量化特征的单词词典
 *
 * @Returns  量化好的直方图 （map<word_idx,frequency>）
 */
/* ------------------------------------------------------------------------*/
Hist_t QuantizeFeature(const vector<vector<float> >& sketch_features,const vector<vector<float> >& vocabulary){
    Hist_t hist;
    priority_queue<Compair_t,std::vector<Compair_t>,Comp> sort_pq;
    for(auto feature : sketch_features){
        // 计算当前feature和每个单词的相似度 
        for(vector<float>::size_type idx = 0; idx < vocabulary.size();++idx){
             auto p = make_pair(idx,CalcSimilarity(vocabulary[idx],feature));
             sort_pq.push(p);
        }
        // 取最相似的单词的下标为当前feature的量,存入map
        std::size_t index = sort_pq.top().first;
        hist[index]++;
    }
    return hist;
}

#endif
