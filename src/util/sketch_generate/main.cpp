/*************************************************************************
 * Copyright (c) 2015 An Boqing
 * All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**************************************************************************/
#include <QApplication>
#include <QImage>
#include <QPixmap>

#include "mainwindow.h"
#include "trianglemesh.h"

#include <boost/filesystem.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstring>
using namespace std;
/*
 * manipulate configuration file with boost::porperty_tree
 */

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


class ConfigSettings
{
public:
    ConfigSettings(const std::string& mdir="./",
                   const std::string& viewdir="./",
                   const std::string& outputdir="./",
                   int viewnum=102):model_dir(mdir),
        view_dir(viewdir),output_dir(outputdir),
        view_num(viewnum) {}

    std::string model_dir;
    std::string view_dir;
    std::string output_dir;
    int view_num;

    bool isDrawBoundaries ;
    bool isDrawOccludingContours ;
    bool isDrawSuggestiveContours ;
    bool isDrawApparentRidges ;
    bool isDrawRidges;
    bool isDrawValleys;

    unsigned resize_height;
    unsigned resize_width;

    // @overwrite
    void load(const std::string & filename);
    void save(const std::string & filename);
};


void ConfigSettings::load(const std::string & filename)
{
    using boost::property_tree::ptree;
    ptree pt;

    //load xml file into the property tree
    read_xml(filename,pt);
    // read from xml file
    model_dir = pt.get<std::string>("settings.model_dir");
    view_dir = pt.get<std::string>("settings.view_dir");
    output_dir = pt.get<std::string>("settings.output_dir",".");
    isDrawBoundaries = pt.get<bool>("settings.isDrawBoundaries",true);
    isDrawOccludingContours = pt.get<bool>("settings.isDrawOccludingContours",false);
    isDrawSuggestiveContours = pt.get<bool>("settings.isDrawSuggestiveContours",true);
    isDrawApparentRidges = pt.get<bool>("settings.isDrawApprentRidges",true);
    isDrawRidges = pt.get<bool>("settings.isDrawRidges",false);
    isDrawValleys = pt.get<bool>("settings.isDrawValleys",false);
    view_num = pt.get("settings.view_num",102);
    resize_height = pt.get("resize_height",256);
    resize_width = pt.get("resize_width",256);
}


void ConfigSettings::save(const std::string& filename)
{
    using boost::property_tree::ptree;
    ptree pt;
    // put property
    pt.put("settings.model_dir",model_dir);
    pt.put("settings.view_dir",view_dir);
    pt.put("settings.output_dir",output_dir);
    pt.put("settings.view_num",view_num);
    write_xml(filename,pt);
}

void usage()
{
    cout << "Usage: gen_view_image path/to/configfile"<<endl;
}

void GetModelFilelist(const string& model_dir,vector<string>& name_vec)
{
    namespace FS = boost::filesystem;
    FS::path p(model_dir);
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
    }
    std::sort(name_vec.begin(),name_vec.end());
}

/*
 * @ file_vec 存储一系列文件的绝对路径名称
 * @ output_root : as its name
 */
void CreateFolderWithFilelist(const std::vector<std::string>& file_vec,
                              std::vector<std::string>& output_vec,
                              const std::string& output_root=".")
{
    using namespace boost::filesystem;
    // need compile with -std=c++11
    for(auto spath : file_vec ) {
        path p(spath);
        //以当前模型文件的父文件夹为输出目录
        //以当前模型文件名为输出文件夹名称
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
                cout << "creating ... " << output_dir.string()<< endl;
                // create_directory_recursively !
                if(!create_directories(output_dir)) {
                    std::cerr << "create path wrong " << output_dir << endl;
                }
            }
        }
    }
}

void ProcessModel(const std::string& model_name,
                  uint view_num,
                  const std::string& view_dir,
                  const std::string& output_dir,
                  TriMeshView& tv)
{

    tv.readMesh(model_name.c_str());
    for(uint ix = 0; ix < view_num; ++ix) {
        // TODO : untest
        std::ostringstream buf;
        buf << view_dir<<"/"<<ix<<".xf";
        tv.readXf(buf.str().c_str());
        //clear buf
        buf.str("");
        // prepair output dir
        buf << output_dir << "/"<<ix<<".jpg";
        QPixmap pixmap = tv.renderPixmap(tv.width(), tv.height());
        pixmap.save(buf.str().c_str());
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(argc < 2) {
        usage();
        exit(1);
    }
    //MainWindow mw;
    //mw.show();

        // read configure info from xml file
    ConfigSettings conf;
    conf.load(argv[1]);
    std::string model_dir = conf.model_dir;
    std::string view_dir = conf.view_dir;
    std::string output_dir = conf.output_dir;
    int view_num = conf.view_num;

    // line drawing style setting
    TriMeshView tv;
    tv.isDrawBoundaries = conf.isDrawBoundaries;
    tv.isDrawOccludingContours = conf.isDrawOccludingContours;
    tv.isDrawSuggestiveContours = conf.isDrawSuggestiveContours;
    tv.isDrawApparentRidges = conf.isDrawApparentRidges;
    tv.isDrawRidges = conf.isDrawRidges;
    tv.isDrawValleys = conf.isDrawValleys;
    tv.resize(conf.resize_height,conf.resize_width);

    std::vector<std::string> file_list,output_list;
    // get model file lsit
    GetModelFilelist(model_dir,file_list);
    // create picture output directories in specified path
    CreateFolderWithFilelist(file_list,output_list,output_dir);
    // process each model
    for(vector<string>::size_type ix = 0; ix < file_list.size(); ++ix){
        string& my_output_dir = output_list[ix];
        string& model_name = file_list[ix];
        ProcessModel(model_name,
                     view_num,
                     view_dir,
                     my_output_dir,
                     tv);
    }
    exit(0);
    return a.exec();
}
