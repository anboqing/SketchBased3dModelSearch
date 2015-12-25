/* Copyright (c) 2015 An Boqing
 * All rights reserved.
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at * http://www.apache.org/licenses/LICENSE-2.0 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * @file file_tool.h
 * @author An Boqing , hunkann@gmail.com
 * @version 1.0.0
 * @date 2015-11-29
 */
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <map>
#include <unordered_map>
#include <regex>

#include <boost/filesystem.hpp>

#include <stdlib.h>

#include <glog/logging.h>

#include "typedefs.h"

#ifndef __FILE_TOOL_H__ 
#define __FILE_TOOL_H__


/* ------------------------------------------------------------------------*/
/**
 * @brief 保存倒排索引
 *
 * @Param filename
 * @Param inverse_index
 */
/* ------------------------------------------------------------------------*/
void SaveIndex(const std::string& filename,
        std::map<std::size_t,std::map<std::size_t,float> > inverse_index){
    std::ofstream ofs(filename);
    if(!ofs.is_open()){
        LOG(FATAL) << " open index file error " ;
    }
    for(auto& wordid_inverselist : inverse_index){
        ofs << wordid_inverselist.first << "\t"; 
        for(auto& docid_tfidf : wordid_inverselist.second){
            ofs << docid_tfidf.first << " " << docid_tfidf.second <<" "; 
        }
        ofs << std::endl;
    }
    ofs.close();
}

/* ------------------------------------------------------------------------*/
/**
 * @brief 读取倒排列表
 *
 * @Param filename
 * @Param inverse_index
 */
/* ------------------------------------------------------------------------*/
void LoadIndex(const std::string& filename,
        sbe::InverseIndex_t& inverse_index){
    std::ifstream ifs(filename);
    if(!ifs.is_open()){
        LOG(FATAL) << " open index file error " ;
    }
    std::string line;
    while(std::getline(ifs,line)){
        std::istringstream iss(line); 
        std::size_t word_index;
        iss >> word_index;
        std::unordered_map<std::size_t,float> inverse_list;
        std::size_t doc_index;
        float tfidf;
        while(iss >> doc_index && iss >> tfidf){
            inverse_list.insert(std::make_pair(doc_index,tfidf)); 
        }
        inverse_index.insert(std::make_pair(word_index,inverse_list));
    }
    ifs.close();
}

/* ------------------------------------------------------------------------*/
/**
 * @brief 存储 <word_id,term-frequency>  单词在vocabulary中的下标和单词在所有文档出现的频率
 *
 * @Param filename
 * @Param term_frequency_counter
 */
/* ------------------------------------------------------------------------*/
void SaveTFC(const std::string& filename,std::map<std::size_t,std::size_t>& term_frequency_counter){
    std::ofstream ofs(filename);
    if(!ofs.is_open()){
        LOG(FATAL) << "open file error "  ; 
        return;
    }
    for(auto pair : term_frequency_counter){
        ofs << pair.first << " " << pair.second << std::endl;
    }
    ofs.close();
}

/* ------------------------------------------------------------------------*/
/**
 * @brief 读取 单词-文档频率到map中
 *
 * @Param filename
 * @Param term_frequency_counter
 */
/* ------------------------------------------------------------------------*/
void LoadTFC(const std::string& filename,std::unordered_map<std::size_t,std::size_t>& term_frequency_counter){
    std::ifstream ifs(filename);
    if(!ifs.is_open()){
        LOG(FATAL) << "open file error " ;
    }
    std::string line;
    while(getline(ifs,line)){
        std::istringstream iss(line);
        std::size_t word_index,word_count;
        iss >> word_index >> word_count;
        term_frequency_counter.insert(std::make_pair(word_index,word_count));
    }
    ifs.close();
}


/* ------------------------------------------------------------------------*/
/**
 * @brief 从特征文件名里面提取出来 模型名称、sketch编号、row 和col，eg. D00001_1_218_64.dat row=218,col=64
 *
 * @Param name : 文件名
 * @Param model_name : 模型文件名
 * @Param sketch_index: 模型生成的草图的编号 1~102
 * @Param row : 特征矩阵的row number
 * @Param col : 特征矩阵的col number
 *
 * @Returns   : status
 */
/* ------------------------------------------------------------------------*/
bool GetMetaByName(const std::string& name,
        std::string& model_name,
        unsigned* sketch_index,
        unsigned* row,
        unsigned *col){
    // 正则表达式提取
    std::smatch results;
    std::regex pattern("(.*)(_)(.*?)(_)(.*?)(_)(.*?)\\.dat"); 
    if(std::regex_match(name,results,pattern)){
        model_name = results[1].str();
        *sketch_index = atoi((results[3]).str().c_str());
        *row = atoi((results[5]).str().c_str());   
        *col = atoi((results[7]).str().c_str());   
        return true;
    }else{
        return false;
    }
}

/* ------------------------------------------------------------------------*/
/**
 * @brief 从模型文件路径名 提取模型文件名
 *
 * @Param name
 * @Param model_name
 *
 * @Returns   
 */
/* ------------------------------------------------------------------------*/
bool GetModelName(const std::string& name,
        std::string& model_name){
    namespace FS = boost::filesystem;
    FS::path p(name);
    if(FS::exists(p)){
        model_name = p.stem().string();
        return true;
    }else{return false;}
}

bool GetSketchModelName(const std::string& sketch_path,
        std::string& model_name){
    namespace FS = boost::filesystem;
    FS::path p(sketch_path);
    if(FS::exists(p)){
        std::string parent_path = p.parent_path().string();
        model_name = parent_path.substr(parent_path.rfind("/")+1,parent_path.length()-1);
        return true;
    }
    else
        return false;
}

/* ------------------------------------------------------------------------*/
/**
 * @brief : read all file names recursively from given `path` into `name_vec`
 *
 * @Param path : path that you want to search
 * @Param name_vec : output parameter , store the filenames in the path
 *
 * @Returns  : status EXIT_FAILURE, EXIT_SUCCESS
 */
/* ------------------------------------------------------------------------*/
int GetFileListInPath(const std::string& path,std::vector<std::string>& name_vec){
    namespace FS = boost::filesystem;
    FS::path p(path);
    if(FS::exists(p)) {
        //如果是文件，就把文件名加入vec
        if(FS::is_regular_file(p)) {
            name_vec.push_back(p.string());
        } else if(FS::is_directory(p)) {
            //如果是文件夹，就遍历之
            FS::recursive_directory_iterator iter(p),end;
            for(; iter!=end; ++iter) {
                if(FS::is_regular_file(iter->status())) {
                    name_vec.push_back(iter->path().string());
                }
            }
        }
        std::sort(name_vec.begin(),name_vec.end());
    }else{
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}



int GetFileListByExtension(const std::string& path,std::vector<std::string>& name_vec,const std::string& extension){
    namespace FS = boost::filesystem;
    FS::path p(path);
    if(FS::exists(p)) {
        //如果是文件，就把文件名加入vec
        if(FS::is_regular_file(p)) {
            if(p.string().compare(extension) == 0)
                name_vec.push_back(p.string());
        } else if(FS::is_directory(p)) {
            //如果是文件夹，就遍历之
            FS::recursive_directory_iterator iter(p),end;
            for(; iter!=end; ++iter) {
                if(FS::is_regular_file(iter->status()) ) {
                    std::string str = (iter->path()).extension().string();
                    if(str.compare(extension) == 0 )
                        name_vec.push_back(iter->path().string());
                }
            }
        }
        std::sort(name_vec.begin(),name_vec.end());
    }else{
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/*
void CreateFolderWithFilelist(const std::vector<std::string>& file_vec,
                              std::vector<std::string>& output_vec,
                              const std::string& output_root=".")
{
    using namespace boost::filesystem;
    // need compile with -std=c++11
    for(auto spath : file_vec ) {
        path p(spath);
        //以当前文件名为输出文件夹名称
        if(p.has_stem() && (strcmp(p.extension().c_str(),".off")==0)) {
            // 拼接输出文件夹地址
            char* output_dir_name;
            //如果指定了输出文件地址
            if(0!=strcmp(output_root.c_str(),"."))
                output_dir_name = const_cast<char*>((path(output_root)/p.stem()).string().c_str());
            else
                output_dir_name =const_cast<char*>((p.parent_path()/p.stem()).string().c_str());
            path output_dir(output_dir_name);
            output_vec.push_back(output_dir.string());
            // 创建文件家
            if(!is_directory(output_dir)) {
                // debug
                LOG(INFO)<< "creating ... " << output_dir.string();
                // create_directory_recursively !
                if(!create_directories(output_dir)) {
                    LOG(WARNING) << "create path wrong " << output_dir.string();
                }
            }
        }
    }
}
*/


/*
 * @brief 读取数据到 cv::Mat
 * @param filename : data file name
 * @param matData <out param> : mat to return data
 * @param matChns : 矩阵通道数
 */
int LoadData2cvMat(std::string fileName, cv::Mat& matData, int matRows = 0, int matCols = 0, int matChns = 1)  
{  
  
    // 打开文件  
    std::ifstream inFile(fileName.c_str(), std::ios_base::in);  
    if(!inFile.is_open())  
    {  
        std::cerr << "读取文件失败" << std::endl;  
        return EXIT_FAILURE;
    }  
  
    // 载入数据  
    std::istream_iterator<float> begin(inFile);    //按 float 格式取文件数据流的起始指针  
    std::istream_iterator<float> end;          //取文件流的终止位置  
    std::vector<float> inData(begin,end);      //将文件数据保存至 std::vector 中  
    cv::Mat tmpMat = cv::Mat(inData);       //将数据由 std::vector 转换为 cv::Mat  
  
    // 输出到命令行窗口  
    //copy(vec.begin(),vec.end(),ostream_iterator<double>(cout,"\t"));   
  
    // 检查设定的矩阵尺寸和通道数  
    size_t dataLength = inData.size();  
    //1.通道数  
    if (matChns == 0)  
    {  
        matChns = 1;  
    }  
    //2.行列数  
    if (matRows != 0 && matCols == 0)  
    {  
        matCols = dataLength / matChns / matRows;  
    }   
    else if (matCols != 0 && matRows == 0)  
    {  
        matRows = dataLength / matChns / matCols;  
    }  
    else if (matCols == 0 && matRows == 0)  
    {  
        matRows = dataLength / matChns;  
        matCols = 1;  
    }  
    //3.数据总长度  
    if (dataLength != (matRows * matCols * matChns))  
    {  
        LOG(INFO)<< "读入的数据长度 不满足 设定的矩阵尺寸与通道数要求，将按默认方式输出矩阵！" << std::endl;  
        matChns = 1;  
        matRows = dataLength;  
    }   
  
    // 将文件数据保存至输出矩阵  
    matData = tmpMat.reshape(matChns, matRows).clone();  
      
    return EXIT_SUCCESS;  
}  



/* ------------------------------------------------------------------------*/
/**
 * @brief 把cv:: Mat 的数据写入到文件中
 * @Param fileName 要写入的文件名
 * @Param matData 数据
 * @Returns  0 if success , -1 if failed
 */
/* ------------------------------------------------------------------------*/
int WriteCVMat2File(std::string fileName, cv::Mat& matData)  
{  
  
    // 打开文件  
    std::ofstream outFile(fileName.c_str(), std::ios_base::out);  //按新建或覆盖方式写入  
    if (!outFile.is_open()) {  
        LOG(FATAL)<< "打开文件失败" << std::endl;   
        return EXIT_FAILURE;
    }  
    // 检查矩阵是否为空  
    if (matData.empty()) {  
        LOG(FATAL) << "矩阵为空" << std::endl;   
        return EXIT_FAILURE;
    }  
    // 写入数据  
    for (int r = 0; r < matData.rows; r++)  {  
        for (int c = 0; c < matData.cols; c++)  {  
            float data = matData.at<float>(r,c);  //读取数据，at<type> - type 是矩阵元素的具体数据格式  
            outFile << data << "\t" ;   //每列数据用 tab 隔开  
        }  
        outFile << std::endl;  //换行  
    }  
    return EXIT_SUCCESS;
}  


/* ------------------------------------------------------------------------*/
/**
 * @brief 获取当前文件的上一级目录的名称
 *
 * @Param file_full_path :　当前文件的完整路径名
 *
 * @Returns  上一级目录的名称　string
 */
/* ------------------------------------------------------------------------*/
std::string GetParentDirectory(const std::string& file_full_path){
    namespace fs = boost::filesystem;
    using std::string;
    fs::path p(file_full_path);
    string parent_path = p.parent_path().string();
    return parent_path.substr(parent_path.rfind("/")+1,parent_path.length());
}

/* ------------------------------------------------------------------------*/
/**
 * @brief 把vector<vector<type> >类型的数据保存到指定文件夹的文件里，文件名 \
 *        是  上一级目录的名称_当前文件名_rows_cols.dat
 *
 * @param Data_T vector的数据类型
 * @Param root_path 要保存的目标文件夹名
 * @Param file_path 当前数据文件完整路径名 eg. /home/somebody/file.png
 * @Param data 要保存的数据
 *
 * @Returns  status
 */
/* ------------------------------------------------------------------------*/
template <typename Data_T>
int WriteVec(const std::string& root_path,
        const std::string& file_path,
        const std::vector<std::vector<Data_T> >& data){
// 先把文件路径名拼接出来
    // 首先用boost::filesytem::path 把文件名从路径中解析出来
    namespace FS = boost::filesystem;
    FS::path file_p(file_path),root(root_path);
    std::string filename(file_p.stem().c_str());
    // 然后把数据的 父目录名（模型名称）　row,col,等元信息写入文件名
    std::ostringstream oss(filename);
    oss<<GetParentDirectory(file_path)<<"_"<<filename<<"_"<<data.size()<<"_"<<data[0].size()<<".dat";
    // 最后和root_path拼接成完整路径名，写入文件
    FS::path meta_path(oss.str());
    root/=meta_path;

    std::ofstream outfile(root.string());
    if(!outfile.is_open()){
        LOG(FATAL)<<"open output feature file failed , file_name " << root.string()<< std::endl;
        return EXIT_FAILURE;
    }
    for(auto vec : data){
        for(auto d : vec){
            outfile << d << " "; 
        } 
        outfile<<std::endl;
    }

    outfile.close();
    return EXIT_SUCCESS;
}


template <typename DATA>
int LoadData2stdVec(const std::string data_path,
        std::vector<std::vector<DATA> >& data){
    std::ifstream infile(data_path);
    if(!infile.is_open()){
        LOG(FATAL) << " file open error ";    
        return -1;
    }
    std::string line;
    while(getline(infile,line)){
        std::istringstream iss(line);
        std::vector<DATA> temp_vec;
        DATA element;
        while(iss >> element){
            temp_vec.push_back(element); 
        }
        data.push_back(temp_vec);
    }
    return 0;
}

/* ------------------------------------------------------------------------*/
/**
 * @brief 把计算出的所有sketch的histogram存到文件
 *
 * @Param data
 * @Param filename
 *
 * @Returns   
 */
/* ------------------------------------------------------------------------*/
template <typename KEY_T,typename VALUE_T>
int SaveHist(const std::vector<std::map<KEY_T,VALUE_T> >& data,const std::string& filename){
    
    std::ofstream ofs(filename);
    if(!ofs.is_open()){
        LOG(FATAL)<< "file open error";
    }

    for(auto hist : data){
        for(auto pair : hist){
            ofs << pair.first << " " << pair.second << " "; 
        } 
        ofs << std::endl;
    }
    
    ofs.close();
    return 0;
}


template <typename KEY_T,typename VALUE_T>
int SaveSingleHist(const std::map<KEY_T,VALUE_T>& data,const std::string& filename){
    
    std::ofstream ofs(filename);
    if(!ofs.is_open()){
        LOG(FATAL)<< "file open error";
    }

    for(auto pair: data){
        ofs << pair.first << " " << pair.second << " "; 
        ofs << std::endl;
    }
    
    ofs.close();
    return 0;
}


/* ------------------------------------------------------------------------*/
/**
 * @brief 把存起来的直方图读取到内存中
 * @Param data
 * @Param filename
 * @Returns   
 */
/* ------------------------------------------------------------------------*/
template <typename KEY_T,typename VALUE_T>
int LoadHist(std::vector<std::map<KEY_T,VALUE_T> >& data,const std::string& filename){
    std::ifstream ifs(filename);
    if(!ifs.is_open()){
        LOG(FATAL)<< "file open error";
    }

    std::string line;
    while(getline(ifs,line)){
        std::istringstream iss(line);        
        std::map<KEY_T,VALUE_T> hist;
        KEY_T doc_id;
        VALUE_T frequency;
        while(iss >> doc_id && iss >> frequency){
            hist[doc_id] = frequency; 
        }
        data.push_back(hist);
    }

    ifs.close();
    return 0;
}

template <typename KEY_T,typename VALUE_T>
int LoadFastHist(std::vector<std::unordered_map<KEY_T,VALUE_T> >& data,const std::string& filename){
    std::ifstream ifs(filename);
    if(!ifs.is_open()){
        LOG(FATAL)<< "file open error";
    }

    std::string line;
    while(getline(ifs,line)){
        std::istringstream iss(line);        
        std::unordered_map<KEY_T,VALUE_T> hist;
        KEY_T doc_id;
        VALUE_T frequency;
        while(iss >> doc_id && iss >> frequency){
            hist[doc_id] = frequency; 
        }
        data.push_back(hist);
    }

    ifs.close();
    return 0;
}

#endif
