#ifndef __KMEANS_PLUS_H__
#define __KMEANS_PLUS_H__

#include <set>
#include <vector>

#include <random>
#include <functional>
#include <limits>

#include <glog/logging.h>
#include <stdlib.h>
#include <cmath>

// 定义一系列距离度量函数
double dist_euclidean(std::vector<float>& a,std::vector<float>& b){
    double res = 0;
    CHECK(a.size()==b.size());
    for(std::size_t idx = 0; idx < a.size() ; ++idx){
        double dis = a[idx]-b[idx];
        res += dis*dis;
    }
    return sqrt(res);
}

/* ------------------------------------------------------------------------*/
/**
 * @brief 用kmeans++ 算法初始化聚类中心
 *
 * @Param centroids : 初始化好的聚类中心
 * @Param all_features : 所有特征集合
 * @Param num_clusters : 聚类中心数量
 * @Param dist : 距离度量函数
 */
/* ------------------------------------------------------------------------*/
template <class dist_func_t>
void KmeansPlusPlusInit(std::vector<std::size_t>& centroids,
        const std::vector<std::vector<float> >& all_features,
        std::size_t num_clusters,
        const dist_func_t& dist_fun){
    CHECK(num_clusters >0); 
    CHECK(all_features.size()>=num_clusters);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    auto unirand_gen = std::bind(distribution,mt);

    std::size_t num_trials = 2+std::log(num_clusters); // 尝试从现存的中心点里选择使得平均距离最小的中心 , 尝试次数

    // 1 .Choose one center uniformly at random from among the data points.
    std::set<std::size_t> centers;
    std::size_t first_cent = unirand_gen() * all_features.size();
    centers.insert(first_cent);

    // 2.For each data point x, compute D(x),
    // the distance between x and the nearest center that has already been chosen.
    std::vector<double> distances(all_features.size()); // 每个feature到已选中心的距离
    double potential = 0.0;
    for(std::size_t idx = 0 ;idx < all_features.size() ; idx ++){
        double d = dist_fun(all_features[first_cent],all_features[idx]); 
        distances[idx] = d*d;
        potential += distances[idx];
    }

    // Reapeat steps 2 and 3 until k centers have been chosen.
    for(std::size_t count = 0;count < num_clusters;count++){
        
        //  尝试从现存的中心点里选择使得平均距离最小的中心 
        double min_potential = std::numeric_limits<double>::max();
        std::size_t best_center_index = 0;
        for(std::size_t idx = 0; idx < num_trials ; idx++){
            std::size_t index;
            // 依照概率找到index对应的区间
            double  r = unirand_gen() * potential;
            for(index = 0; index < all_features.size() && r > distances[index]; index ++){
                r-=distances[index]; 
            }

            // 只要index 已经被选择了，就线性平移index,直到找到一个没有被选择的index;
            while(centers.count(index) > 0)
                index = (index+1)% all_features.size();

            // 当前的index对应的点all_features[index]就是新的中心点

            // 重新计算potential
            double p = 0.0;
            for(std::size_t idx = 0; idx < all_features.size() ; idx ++){
                double d = dist_fun(all_features[index],all_features[idx]);
                p+= std::min(distances[idx],d*d); // 和旧有的distance比较，取较小者
            }
            if(p<min_potential){
                min_potential = p;
                best_center_index = index;
            }
        }

        for(std::size_t idx = 0; idx < all_features.size() ;idx ++){
            double d = dist_fun(all_features[best_center_index],all_features[idx]); 
            distances[idx] = d*d;
        }

        potential = min_potential;

        centers.insert(best_center_index);
    }
    std::copy(centers.begin(),centers.end(),std::back_inserter(centroids));
}
        
    


#endif
