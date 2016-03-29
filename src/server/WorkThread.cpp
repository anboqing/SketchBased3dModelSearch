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
#include <malloc.h>

#include <glog/logging.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>


void process_task(Task& t){
    LOG(INFO) << t.sockfd_ << " is being processing";
}

void WorkThread::working(){
    Task t;
    for(;;){
        DLOG(INFO)<< this->get_tid() << " is working ";
        thread_manager_->get_task(&t);
        process_task(t);
    }
}
