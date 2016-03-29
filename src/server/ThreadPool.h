/*
 * =====================================================================================
 *
 *       Filename:  ThreadPool.h
 *
 *    Description:  简易的线程池
 *
 *        Version:  1.0
 *        Created:  2016年03月26日 22时42分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <queue>
#include <vector>

#include "Task.h"
#include "WorkThread.h"

#include "Condition.h"
#include "Mutex.h"


class ThreadPoolManager{
public:
    ThreadPoolManager(const size_t size);
    ~ThreadPoolManager();
    // 任务增加／获取
    void add_task(const Task& t);
    void get_task(Task *t);
    
    void start();

private:
    // 禁止复制行为
    ThreadPoolManager(const ThreadPoolManager&);
    ThreadPoolManager& operator=(const ThreadPoolManager&){return *this;}

    void stop();
private:
    std::vector<WorkThread> work_thread_vector_; // 工作线程们
    std::queue<Task> task_queue_;               // 任务队列

    MutexLock mutex_;      // 临界区互斥锁
    MutexLock cond_lock_;
    Condition cond_var_; // 条件变量

    bool pool_is_open_;
    size_t pool_size_;

};




#endif
