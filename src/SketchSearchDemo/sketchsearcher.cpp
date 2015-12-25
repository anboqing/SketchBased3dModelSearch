#include "sketchsearcher.h"
#include "file_tool.h"
#include "config.h"
#include "galif.h"
#include "quantize.h"
#include "Timer.h"

using namespace std;

SketchSearcher* SketchSearcher::_instance  =nullptr;

SketchSearcher* SketchSearcher::getInstance(){
    if(_instance == nullptr){
        _instance = new SketchSearcher("./path.conf");
    }
    return _instance;
}

SketchSearcher::SketchSearcher(const std::string& path):
    _numOfResults(20),_numOfViews(102)
{
    // 读取所有路径
    std::shared_ptr<PathConfig> pconf = PathConfig::GetInstance();
    pconf->load(path);
    // 获取草图文件列表
    const string& sketch_path = pconf->_conf_map["sketch_path"];
    GetFileListInPath(sketch_path,sketch_filename_list);

    // 获取模型文件列表
    const string& model_path = pconf->_conf_map["model_path"];
    GetFileListInPath(model_path,model_filename_list);

    // 建立草图-模型映射
    size_t idcnt = 0;
    for(auto& model_name_path : model_filename_list){
        string model_name;
        GetModelName(model_name_path,model_name);
        model_name_map.insert(make_pair(model_name,idcnt++));
    }

    for(vector<string>::size_type idx = 0 ;idx < sketch_filename_list.size() ; ++ idx ){
        string sketch_model_name ;
        GetSketchModelName(sketch_filename_list[idx],sketch_model_name);
        sketch_model_mapping.insert(make_pair(idx,model_name_map[sketch_model_name]));
    }

    shared_ptr<FeatureConfig> fconf = FeatureConfig::GetInstance();
    const std::string& feature_conf_path = pconf->_conf_map["feature_conf_path"];
    fconf->load(feature_conf_path);
    p_galif = make_shared<sbe::Galif>(*(new sbe::Galif(*fconf)));

// 读取倒排索引
    const std::string& index_path = pconf->_conf_map["index_path"];
    LoadIndex(index_path,inverse_index);

// 读取每篇文档的直方图
    const std::string& hist_path_tfidf = pconf->_conf_map["hist_path_tfidf"];
    LoadFastHist<std::size_t,float>(hist_tfidf,hist_path_tfidf);
// 读取每个单词的term_frequency
    const std::string& tfc_path = pconf->_conf_map["tfc_path"];
    LoadTFC(tfc_path,term_frequency_map);
// 读取单词表
    const string& vocabulary_path = pconf->_conf_map["vocabulary_path"];
    LoadData2stdVec(vocabulary_path,vocabulary);

}

void SketchSearcher::query(const std::string &fileName, QueryResults &results)
{
    //extract features
    cv::Mat query_img = cv::imread(fileName.c_str());

    // 提取查询草图特征
       sbe::KeyPoints_t keypoints;
       sbe::Features_t features; //查询草图的特征

       p_galif->compute(query_img,keypoints,features);
       Hist_t query_hist = QuantizeFeature(features,vocabulary);

       // -在倒排索引中提取包含查询词的所有文档集合
       size_t sum_wordcount=0; // 查询草图的单词数量
       unordered_set<size_t> modelidx_set; // 用来过滤已经加入的模型对应的草图

       for(auto& wordid_wc : query_hist){
           // 取出查询特征里的每个单词
           size_t word_id = wordid_wc.first;
           sum_wordcount+=wordid_wc.second;
           // 在倒排索引中查找当前单词的倒排列表
           unordered_map<size_t,float>& inverse_list = inverse_index[word_id];
           // 遍历当前倒排列表，把其中的所有文档id拿出来存入候选doc集合
           for(auto& docid_weigth : inverse_list){
               // 进行过滤,若该文档对应的model已经加入了，就不加入该文档
               size_t model_idx = sketch_model_mapping[docid_weigth.first];
               if(modelidx_set.count(model_idx) == 0 ){
                   modelidx_set.insert(model_idx);
                   candidate_doc_set.insert(docid_weigth.first);
               }
           }
       }
       // -生成查询特征向量 hj=(hj/sum_hj)log(N/fj)
       unordered_map<size_t,float> query_feature_vector;
       size_t N = hist_tfidf.size();// 总的文档数
       for(auto& wordid_wc : query_hist){
           // 提取当前单词在所有文档出现的次数
           float term_frequency =static_cast<float>(term_frequency_map[wordid_wc.first]);
           float word_count = static_cast<float>(wordid_wc.second);
           float tfidf = (word_count/sum_wordcount)*log(N/(term_frequency));
           query_feature_vector.insert(std::make_pair(wordid_wc.first,tfidf));
       }

       // --把候选文档集合的histogram和查询特征向量求相似度，排序，返回结果
       priority_queue<Compair_t,std::vector<Compair_t>,Comp> sort_pq;

       for(auto doc_id : candidate_doc_set){
            // 获取一个候选文档特征向量
            unordered_map<size_t,float>& candidate_feature_vector = hist_tfidf[doc_id];
            // 和查询特征向量求相似度
            double simi = CalcMapSimilarity(query_feature_vector,candidate_feature_vector);
            // 把docid,simi存入优先级队列进行排序
            sort_pq.push(make_pair(doc_id,simi));
       }


    results.resize(sort_pq.size());

    int count = _numOfResults;
    for(size_t i= 0;sort_pq.size()!=0 && count--;i++) {
        auto& p = sort_pq.top();
        results[i].ratio = p.second;
        results[i].imageIndex = p.first;
        results[i].imageName = sketch_filename_list[p.first];
        sort_pq.pop();
    }
}
