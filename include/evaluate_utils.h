/**
 * @file evaluate_utils.h
 * @brief functions for evaluate
 * @author An Boqing , hunkann@gmail.com
 * @version 1
 * @date 2016-02-11
 */

#ifndef __EVALUATE_UTIL__
#define __EVALUATE_UTIL__

#include <map>
#include <fstream>
#include <sstream>
#include <vector>

typedef size_t Index_t;

int ReadClassificationFile(const std::string& filename,
        std::map<Index_t,std::string>& model_class,
        std::map<std::string,size_t>& cla_info){
    std::ifstream infile;
    infile.open(filename.c_str());
    if(!infile.is_open()){
        return -1;
    }
    std::string line;
    std::getline(infile,line);
    
    int class_num,model_num;
    std::getline(infile,line);
    std::istringstream sline(line);
    sline >> class_num >> model_num;

    for(size_t i = 0;i<class_num;++i){
        std::string class_name,parent_cls_name;
        size_t model_num_class;
        std::getline(infile,line); // pass the empty line
        std::getline(infile,line);
        std::istringstream sline(line);

        sline >> class_name;
        sline >> parent_cls_name;
        sline >> model_num_class;
        cla_info[class_name] = model_num_class; 
        for(size_t j = 0; j < model_num_class;++j){
            size_t model_index;
            std::getline(infile,line);
            std::istringstream ssline(line);
            ssline >> model_index;
            model_class[model_index] = class_name;
        }
    }

    infile.close();
    return 0;
}

void GetModelNameIndexDiction(
        std::map<std::string,size_t>& class_info,
    std::map<std::string,size_t>& model_name_index){
    size_t count = 0;
    for(auto& name_num:class_info){
         model_name_index[name_num.first] = count++;
    }
}

void GenModelClassList(std::map<size_t,std::string>& model_class,
        std::map<std::string,size_t>& cla_info,
        std::vector<std::vector<size_t> >& model_class_v){
        std::map<std::string,size_t> name_dic;
        GetModelNameIndexDiction(cla_info,name_dic);
        for(auto& p : model_class){
            std::vector<size_t> ivec;
            ivec.push_back(p.first);
            ivec.push_back(name_dic[p.second]);
            model_class_v.push_back(ivec);
        }
}


#endif

