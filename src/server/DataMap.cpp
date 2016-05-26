/*
 * =====================================================================================
 *
 *       Filename:  DataMap.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月29日 09时35分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "DataMap.h"

DataMap* DataMap::instance_ = NULL;
MutexLock DataMap::lock_;

DataMap* DataMap::getInstance(){
    if(instance_ == NULL){
        LockGuarde lock(lock_);
        if(instance_ == NULL){
            instance_ = new DataMap();
        }
    }
    return instance_;
}

void DataMap::setData(int fd,const std::string& data){
    LockGuarde lock(lock_);
    data_map_[fd] = data;
}

std::string DataMap::getData(int fd){
    std::string res;
    {
        LockGuarde lock(lock_);
        res = data_map_[fd];
    }
    return res;
}
