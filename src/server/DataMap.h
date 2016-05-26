/*
 * =====================================================================================
 *
 *       Filename:  DataMap.h
 *
 *    Description:  存放每个连接的数据，全局共享，单例模式  
 *
 *        Version:  1.0
 *        Created:  2016年03月29日 09时13分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _DATAMAP_H_
#define _DATAMAP_H_

#include <unordered_map> 
#include <string>
#include "Mutex.h"

class DataMap{
public:
    static DataMap* getInstance();

    void setData(int fd,const std::string& data);

    std::string getData(int fd);

    static MutexLock lock_;
private:
    DataMap():data_map_(){}
    ~DataMap(){delete instance_;}
private:

    std::unordered_map<int,std::string> data_map_;

    static DataMap *instance_;
};



#endif
