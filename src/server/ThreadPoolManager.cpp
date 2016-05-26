/*
 * =====================================================================================
 *
 *       Filename:  ThreadPoolManager.cpp
 *
 *    Description:  ThreadPoolManager的实现文件
 *
 *        Version:  1.0
 *        Created:  2016年03月27日 18时17分06秒
 *       Revision:  none
 *       Compiler:  gcc -std=c++11
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *
 * =====================================================================================
 */

#include "ThreadPool.h"
#include "WorkThread.h"
#include "Task.h"

#include <glog/logging.h>

using namespace std;

ThreadPoolManager::ThreadPoolManager(const size_t size):
    work_thread_vector_(size),
    task_queue_(),
    mutex_(),
    cond_lock_(),
    cond_var_(&cond_lock_),
    pool_is_open_(false),
    pool_size_(size)
{
    // 给所有工作线程注册线程池指针，
    // 使工作线程可以通过该指针使用任务队列
    for(auto& iter : work_thread_vector_){
        iter.reg_pool(this);
    }
}

void ThreadPoolManager::start(){
    LockGuarde lock(mutex_);
    if(pool_is_open_ == true){
        return;
    }
    std::vector<WorkThread>::iterator thread = work_thread_vector_.begin();
    for(;thread!=work_thread_vector_.end();++thread){
        thread->start();
    }
    pool_is_open_ = true;
}

void ThreadPoolManager::stop(){
    if(!pool_is_open_){
        return;
    }

    // 唤醒所有阻塞的线程
    cond_var_.notify_all();

    {
        LockGuarde lock(mutex_);
        pool_is_open_ = false;
    }

    for(auto& iter : work_thread_vector_){
        iter.exit();
    }

    for(;;){
        LockGuarde lock(mutex_);
        if(!task_queue_.empty()){
            task_queue_.pop();
        }else{
            break;
        }
    }
}

ThreadPoolManager::~ThreadPoolManager(){
    // 析构线程池
    stop();
}

void ThreadPoolManager::add_task(const Task& t){
    //LockGuarde lock(mutex_);
    DLOG(INFO) << "add task ..";
    if(!pool_is_open_){
        //mutex_.unlock();
        LOG(ERROR) << "Thread pool is not open";
        return;
    }
    DLOG(INFO) << " notify worker " ;
    task_queue_.push(t);
    // 唤醒一个等待的进程
    cond_var_.notify();
}

void ThreadPoolManager::get_task(Task *t){
    LockGuarde lock(mutex_);
    if(!pool_is_open_){
        mutex_.unlock();
        LOG(ERROR) << "Thread Pool is not open";
        return;
    }
    DLOG(INFO) << "wait task .. ";
    // 任务队列为空就等待
    while(task_queue_.size() == 0){
        cond_var_.wait();
    }
    // 任务队列不为空就取出一个任务
    (*t) = task_queue_.front();
    task_queue_.pop();
    DLOG(INFO) << "get task complete ";
}
