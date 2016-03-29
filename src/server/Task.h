/*
 * =====================================================================================
 *
 *       Filename:  Task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月27日 18时47分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _TASK_H_
#define _TASK_H_

#include <string>


class Task{
public:
    enum task_type{
        ReadTask,WriteTask
    };
    int sockfd_;
    unsigned int data_size_; 
    task_type type_;
    std::string data_;
    Task& operator=(const Task& t){
        sockfd_ = t.sockfd_;
        data_size_ = t.data_size_;
        data_ = t.data_;
        return *this;
    }
};




#endif
