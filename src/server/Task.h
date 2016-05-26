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

    Task():sockfd_(5),epollfd_(4){}

    Task(int fd,int epollfd):sockfd_(fd),epollfd_(epollfd){}

    int sockfd_;
    int epollfd_;

    Task& operator=(const Task& t){
        sockfd_ = t.sockfd_;
        epollfd_ = t.epollfd_;
        return *this;
    }
};




#endif
