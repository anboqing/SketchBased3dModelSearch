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

#include "file_tool.h"
#include "config.h"
#include "typedefs.h"
#include "galif.h"
#include "quantize.h"
#include <sys/socket.h> // socket listen bind connect
#include <sys/types.h>
#include <sys/wait.h> // wait()
#include <netinet/in.h> // struct sockaddr_in
#include <stdlib.h> // getenv
#include <stdio.h>
#include <signal.h> // signal
#include <sys/epoll.h> // epoll function
#include <fcntl.h> // non-blocking

#include <gflags/gflags.h> // commandline arguments parsing util
#include <glog/logging.h> // logging util 

#include "io_util.h"
#include <iostream>
#include <vector>
using namespace std;
using namespace sbe;
using namespace cv;

const int BUFSIZE = 1024*1024*10;

DEFINE_int32(listen_port,9999,"Port to be listened");
DEFINE_int32(backlog,1024,"The size of listen queue");
DEFINE_int32(pool_size,10,"threads num of the thread pool ");
DEFINE_int32(canvas_width,512,"canvas width");
DEFINE_int32(canvas_height,512,"canvas height");
DEFINE_string(config_path,"../../config/path.conf","path config file");

int listenfd;

void sigact(int fd){
    close(listenfd);
    exit(-1);
}
void sig_child(int signo){
    pid_t pid;
    int stat = 0;
    pid = wait(&stat);
    LOG(INFO) << "child " << pid << " terminated with " << signo;
    return;
}

int setnonblocking(int sockfd){
    if( fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFD,0) | O_NONBLOCK) == -1){
        LOG(ERROR) << "fcntl:" << strerror(errno);
        return -1;
    }
    return 0;
}


int main(int argc,char **argv){

    google::ParseCommandLineFlags(&argc,&argv,true);


    // 读取所有路径
    std::shared_ptr<PathConfig> pconf = PathConfig::GetInstance(); 
    pconf->load(FLAGS_config_path);

    /***********************************************************/
    InverseIndex_t inverse_index; // 倒排索引
    vector<unordered_map<size_t,float> > hist_tfidf; // 文档向量表
    unordered_map<size_t,size_t> term_frequency_map; // 单词的文档频率 word_index,word_count
    vector<vector<float> > vocabulary; // 单词表
    shared_ptr<sbe::Galif> p_galif; //特征提取器
    unordered_set<size_t> candidate_doc_set; // 包含查询词的文档index集合
    //vector<Hist_t> candidate_hists; // 包含查询词的文档hist集合
    vector<string> sketch_filename_list; // 草图文件名列表
    vector<string> model_filename_list; // 模型文件名表
    unordered_map<size_t,size_t> sketch_model_mapping; // 草图-模型映射
    /***********************************************************/

    // 获取草图文件列表
    const string& sketch_path = pconf->_conf_map["sketch_path"];
    GetFileListInPath(sketch_path,sketch_filename_list);

    // 获取模型文件列表
    const string& model_path = pconf->_conf_map["model_path"];
    GetFileListInPath(model_path,model_filename_list);

    // 建立草图-模型映射
    // 获取模型文件名 和 草图文件的父文件夹 比较
    unordered_map<string,size_t> model_name_map;
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


    LOG(INFO) << " loading data .. " ;

    shared_ptr<FeatureConfig> fconf = FeatureConfig::GetInstance();
    const std::string& feature_conf_path = pconf->_conf_map["feature_conf_path"];
    fconf->load(feature_conf_path);
    p_galif = make_shared<sbe::Galif>(*(new sbe::Galif(*fconf))); 

    // 读取倒排索引
    const std::string& index_path = pconf->_conf_map["index_path"];
    LoadIndex(index_path,inverse_index);
    CHECK(inverse_index.size()!=0);
    LOG(INFO) << " inverse index size : " << inverse_index.size() ;
    // 读取每篇文档的直方图
    const std::string& hist_path_tfidf = pconf->_conf_map["hist_path_tfidf"];
    LoadFastHist<std::size_t,float>(hist_tfidf,hist_path_tfidf);
    LOG(INFO) << " histogram size: " << hist_tfidf.size();
    // 读取每个单词的term_frequency
    const std::string& tfc_path = pconf->_conf_map["tfc_path"];
    LoadTFC(tfc_path,term_frequency_map);
    LOG(INFO) << "term frequency size : " << term_frequency_map.size();
    // 读取单词表
    const string& vocabulary_path = pconf->_conf_map["vocabulary_path"];
    LoadData2stdVec(vocabulary_path,vocabulary);
    LOG(INFO) << "vocabulary size : "<<vocabulary.size();

    // ---------------------------------------------------------------------

    int iret;
    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        LOG(ERROR) << "create listen socket error";
        return -1;
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(FLAGS_listen_port);

    // bind
    if( (iret = bind(listenfd,(const struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0 ){
        LOG(ERROR) << "bind error" << strerror(errno);
        return -1;
    }

    // listen
    int backlog = FLAGS_backlog;
    char *ptr = getenv("LISTENQ");
    if(ptr!=0)
        backlog = atoi(ptr);
    if(listen(listenfd,backlog) < 0){
        LOG(ERROR) << "listen error" << strerror(errno);
        return -1;
    }


    signal(SIGINT,sigact);
    signal(SIGCHLD,sig_child);
    signal(SIGPIPE,SIG_IGN);

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    pid_t pid;

    DLOG(INFO) << " server is listening :" << FLAGS_listen_port ;

    for(;;){
        addr_len = sizeof(sockaddr_in);
        if( (connfd = accept(listenfd,(struct sockaddr*)&client_addr,&addr_len)) < 0){
            if(errno == EINTR) continue;
            else LOG(ERROR) << "accept error";
        }

        DLOG(INFO) << "a client connected ";

        if( (pid = fork()) == 0){
            DLOG(INFO) << "child run ";
            close(listenfd);
            // read image
            char *buf = new char[BUFSIZE];
            memset(buf,0,BUFSIZE);
            DLOG(INFO) << "start recv ...";
            recv(connfd,(void*)buf,BUFSIZE,0);   
            DLOG(INFO) << "end recv ...";
            // query ---------------------------------------------------
            string str(buf);
            istringstream iss(str);
            char* buff = new char[str.size()];
            strcpy(buff,str.c_str());
            free(buf);

            vector<uint8_t> data_vec;
            char *pch;
            pch = strtok(buff," ");
            data_vec.push_back(static_cast<uint8_t>(atoi(pch)));
            while(pch!=NULL){
                uint8_t n = static_cast<uint8_t>(atoi(pch));
                data_vec.push_back(n);
                pch = strtok(NULL," ");
            }
            DLOG(INFO) << "data_vec.size()" << data_vec.size();
            // convert data_vec to uint8_t array
            uint8_t data[data_vec.size()];
            for(size_t i = 0; i< data_vec.size();++i){
                data[i] = 255-data_vec[i];
            }

            DLOG(INFO) << "create query_image";
            
            cv::Mat query_img = cv::Mat(FLAGS_canvas_width,FLAGS_canvas_height,CV_8UC4,data).clone();
            
            DLOG(INFO) << " begin transform query_image";

            cv::cvtColor(query_img,query_img,CV_RGB2BGR);

            // 提取查询草图特征
            sbe::KeyPoints_t keypoints;
            sbe::Features_t features; //查询草图的特征

            DLOG(INFO) << " computing query feature .. " ;

            p_galif->compute(query_img,keypoints,features);
            // quantize查询草图生成查询histogram
            DLOG(INFO) << " quantizing query feature ... " ;
            Hist_t query_hist = QuantizeFeature(features,vocabulary);     
            // -在倒排索引中提取包含查询词的所有文档集合
            DLOG(INFO) << " retrieve candidate doc set .. " ;

            size_t sum_wordcount=0; // 查询草图的单词数量
            unordered_set<size_t> modelidx_set; // 用来过滤已经加入的模型对应的草图

            for(auto& wordid_wc : query_hist){
                // 取出查询特征里的每个单词
                size_t word_id = wordid_wc.first; 
                sum_wordcount+=wordid_wc.second;
                // 在倒排索引中查找当前单词的倒排列表
                unordered_map<size_t,float>& inverse_list = inverse_index[word_id];
                CHECK(inverse_list.size()!=0);
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

            DLOG(INFO) << " generate query feature vector ...";
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
            DLOG(INFO) << " calculate similarity with each candidate doc.. ";
            priority_queue<Compair_t,std::vector<Compair_t>,Comp> sort_pq;
            CHECK(candidate_doc_set.size()!=0);

            for(auto doc_id : candidate_doc_set){
                // 获取一个候选文档特征向量
                unordered_map<size_t,float>& candidate_feature_vector = hist_tfidf[doc_id];
                // 和查询特征向量求相似度
                double simi = CalcMapSimilarity(query_feature_vector,candidate_feature_vector);
                // 把docid,simi存入优先级队列进行排序
                sort_pq.push(make_pair(doc_id,simi));
            }
            // 返回最相似的文档
            size_t count = 10;
            while(sort_pq.size()!=0 && count--){
                auto p = sort_pq.top(); 
                cout << model_filename_list[sketch_model_mapping[p.first]] << "\t\t"<< p.second<<  endl;
                sort_pq.pop();
            }
            free(buff);
            // ------------------------------------------------

            char buffer[1024];
            sprintf(buffer," hello client %d", connfd);
            // return result
            send(connfd,(void*)buffer,strlen(buffer),0);

            exit(0);  
        }
        else if(pid > 0){
            DLOG(INFO) << " parent run ";
        }else{
            DLOG(INFO) << " fork error ";
        }
    }
}



