#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <vector>
#include <map>
#include <unordered_map> // 倒排索引
#include <queue>
#include <string>
#include <set>
#include <cmath>
#include <memory> // std::shared_ptr

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


namespace sbe{
// 定义了整个工程要使用的各种数据类型以及typedefs
using std::vector;
using std::string;
using std::set;

using cv::Mat;

using boost::property_tree::ptree;

//  将无符号整数起别名 uint
typedef unsigned int uint;
// 定义64位整数为 索引下标类型
typedef int64_t Index_t;
// 定义索引向量为 Vec_Index_t
typedef std::vector<Index_t> Vec_Index_t;
// 定义32位浮点数向量为 Vec_f32_t 
typedef std::vector<float> Vec_f32_t;
// 定义关键点类型，二维矩阵
typedef std::vector<Vec_f32_t> KeyPoints_t;
// 定义特征类型为 数据类型为 float 的二维嵌套向量，是每一行是一个样本的特征向量，整个组合起来是所有特征向量
typedef std::vector<Vec_f32_t> Features_t;
// 视觉字典的数据结构，也是一个二维矩阵，一行一个视觉单词的特征
typedef std::vector<Vec_f32_t> Vocabularys_t;
// 被量化后的所有样本的矩阵，每行一个样本
typedef std::vector<Vec_f32_t> Samples_t; //files has been quantized.
// TODO:结果项：是查询的时候用的，第一个float为相似度，第二个为模型的索引
typedef std::pair<float, Index_t> ResultItem_t;
// TODO: 属性树：是解析配置文件用的
typedef boost::property_tree::ptree PropertyTree_t;

// 直方图结构
typedef std::map<std::size_t,std::size_t> Hist_t;
// 倒排索引结构
typedef std::unordered_map<std::size_t,std::map<std::size_t,float> > InverseIndex_t;

} //namespace sbe


#endif
