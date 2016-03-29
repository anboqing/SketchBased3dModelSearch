/*
 * =====================================================================================
 *
 *       Filename:  Thread.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月27日 20时07分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "Thread.h"
#include <glog/logging.h>
#include <errno.h>
#include <stdlib.h>
#include <stdexcept>


Thread::Thread():tid_(pthread_self()){
}

Thread::~Thread(){
    pthread_exit(static_cast<void*>(&tid_));
}

void* Thread::thread_func(void *arg){
    Thread *p_thread = static_cast<Thread*>(arg);
    // 使用动态绑定调用子类实现的工作代码
    p_thread->working();
    return NULL;
}

void Thread::start(){
     if(pthread_create(&tid_,NULL,Thread::thread_func,this) != 0){
        LOG(ERROR) << "pthread_create:" << strerror(errno);
        throw std::runtime_error(strerror(errno));
     }
     LOG(INFO) << "thread " << tid_ << " start " ;
}

void Thread::join(pthread_t tid){
    pthread_join(tid,NULL);
}

void Thread::exit(){
    pthread_exit((void*)tid_);
}
