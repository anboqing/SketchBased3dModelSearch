


#include "typedefs.h"

#ifndef __QUANTIZE_H__
#define __QUANTIZE_H__

#include <map>
#include <queue>
#include <vector>
#include <glog/logging.h>
#include <math.h>
#include <limits>
#include <unordered_map>


using namespace std;

typedef unsigned int Index_t;
typedef std::pair<std::size_t,double> Compair_t;
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


float CalcInnerProduct(const vector<float>& lhs,const vector<float>& rhs){
    float sum  = 0.0;
    for(vector<float>::size_type ix = 0;ix < lhs.size() ;++ix){
        sum += lhs[ix]*rhs[ix]; 
    }
    return sum;
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
        DLOG(FATAL) << " feature vector is empty feature !" ;
    }
    return dot/(len);
}



double CalcMapInnerProduct(std::unordered_map<size_t,float>& lhs,std::unordered_map<size_t,float>& rhs){
    double product = 0.0;
    if(lhs.size() < rhs.size()){
        for(auto& pair : lhs){
            float val;
            if((val = rhs[pair.first]) != 0)
                product+= pair.second * val; 
        }
    }
    else{
        for(auto& pair : rhs){
            float val;
            if((val = lhs[pair.first]) != 0)
                product+= pair.second * val;
        } 
    }
    return product;
}

// 计算特征向量的模
double CalcMapLength(std::unordered_map<size_t,float>& sparse_feature_vector){
    double sum = 0.0;
    for(auto elem : sparse_feature_vector){
        double w = static_cast<double>(elem.second);
        sum += w * w; 
    }
    return sqrt(sum);
}


double CalcMapSimilarity(std::unordered_map<size_t,float>& lhs,std::unordered_map<size_t,float>& rhs){
    double dot,len;
    len = CalcMapLength(lhs)*CalcMapLength(rhs);
    dot = CalcMapInnerProduct(lhs,rhs);
    if(len==0){
        DLOG(FATAL) << " feature vector is empty feature !" ;
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
/*
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
*/
/*
Hist_t QuantizeFeature(const vector<vector<float> >& sketch_features,const vector<vector<float> >& vocabulary){

    Hist_t hist;

    size_t index = 0;
    float min_simi = std::numeric_limits<float>::max();

    for(auto feature : sketch_features){
        // 计算当前feature和每个单词的相似度 
        for(vector<float>::size_type idx = 0; idx < vocabulary.size();++idx){
             float simi = CalcSimilarity(vocabulary[idx],feature);
             if(simi < min_simi){
                min_simi = simi;
                index = idx;
             }
        }
        // 取最相似的单词的下标为当前feature的量,存入map
        hist[index]++;
    }
    return hist;
}
*/


// 定义各种距离公式

/**
 * @brief 计算两个特征点之间的欧式距离(公式见论文p4右下角）
 *  q_ij = arg min || x_i - c_j ||
 */
float Distance(const vector<float>& lhs,const vector<float>& rhs){
    float distance = 0.0;
    for(vector<float>::size_type idx = 0; idx < lhs.size() ;++idx){
        float d = static_cast<float>(lhs[idx]) - static_cast<float>(rhs[idx]);
        distance += d*d; 
    }
    return sqrt(distance);
}

Hist_t QuantizeFeature(const vector<vector<float> >& sketch_features,const vector<vector<float> >& vocabulary){

    Hist_t hist;

    size_t index = 0;

    for(auto feature : sketch_features){
        float min_simi = std::numeric_limits<float>::max();
        // 计算当前feature和每个单词的相似度 
        for(vector<float>::size_type idx = 0; idx < vocabulary.size();++idx){
             //float simi = CalcSimilarity(vocabulary[idx],feature);
             float simi = Distance(vocabulary[idx],feature);
             if(simi < min_simi){
                min_simi = simi;
                index = idx;
             }
        }
        // 取最相似的单词的下标为当前feature的量,存入map
        hist[index]++;
    }
    return hist;
}
#endif
