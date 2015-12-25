#ifndef SKETCHSEARCHER_H
#define SKETCHSEARCHER_H

#include "searchengine.h"

#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>

#include "typedefs.h"
#include "galif.h"




class SketchSearcher : public SearchEngine
{
public:

        void query(const std::string &fileName, QueryResults &results);
public:

    std::vector<std::string>& get_model_filename_list(){return model_filename_list;}
    std::unordered_map<std::size_t,std::size_t>& get_sketch_model_mapping(){return sketch_model_mapping;}

private:

    SketchSearcher(const std::string& path);
    /***********************************************************/
    sbe::InverseIndex_t inverse_index; // 倒排索引
    std::vector<std::unordered_map<std::size_t,float> > hist_tfidf; // 文档向量表
    std::unordered_map<std::size_t,std::size_t> term_frequency_map; // 单词的文档频率 word_index,word_count
    std::vector<std::vector<float> > vocabulary; // 单词表
    std::shared_ptr<sbe::Galif> p_galif; //特征提取器
    std::unordered_set<std::size_t> candidate_doc_set; // 包含查询词的文档index集合
    std::vector<std::string> sketch_filename_list; // 草图文件名列表
    std::vector<std::string> model_filename_list; // 模型文件名表
    std::unordered_map<std::size_t,std::size_t> sketch_model_mapping; // 草图-模型映射
    // 获取模型文件名 和 草图文件的父文件夹 比较
    std::unordered_map<std::string,std::size_t> model_name_map;
    /***********************************************************/


    const unsigned int _numOfResults;
    const unsigned int _numOfViews;
    static SketchSearcher* _instance;
public:
    static SketchSearcher* getInstance();
};

#endif // SKETCHSEARCHER_H
