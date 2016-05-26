/*
 * =====================================================================================
 *
 *       Filename:  WorkThread.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月27日 20时18分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "WorkThread.h"
#include "ThreadPool.h"
#include "Task.h"
#include "DataMap.h"

#include <malloc.h>
#include <glog/logging.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

void process_task(Task& t){
    LOG(INFO) << t.sockfd_ << " is being processing";
    // 从hash_table里取得相应fd的数据，进行处理
    DataMap* pdata = DataMap::getInstance();
    string data = pdata->getData(t.sockfd_);
    // do some thing
    data.append("=== hello client ");
    // 处理完成之后把当前的fd加入epoll的EPOLLOUT，
    // 告知epoll给客户发送数据
    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.fd = t.sockfd_;
    epoll_ctl(t.epollfd_,EPOLL_CTL_MOD,t.sockfd_,&ev);
    // epoll就从hash_table里用fd获取数据，发送出去
    //send(t.sockfd_,(void*)data.c_str(),data.size(),0);
}

void WorkThread::working(){
    Task t;
    for(;;){
        DLOG(INFO)<< this->get_tid() << " is working ";
        thread_manager_->get_task(&t);
        process_task(t);
    }
}
