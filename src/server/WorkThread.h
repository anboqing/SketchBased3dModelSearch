/*
 * =====================================================================================
 *
 *       Filename:  WorkThread.h
 *
 *    Description:  工作线程  
 *
 *        Version:  1.0
 *        Created:  2016年03月27日 18时18分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _WORKTHREAD_H_
#define _WORKTHREAD_H_

#include "Thread.h"



class ThreadPoolManager;

class WorkThread : public Thread {
public:
    void reg_pool(const ThreadPoolManager*); // 用于线程池管理器创建线程时把管理器指针传给工作线程
    void working();
private:
    ThreadPoolManager* thread_manager_; // 管理本线程的线程池对象
};

inline void WorkThread::reg_pool(const ThreadPoolManager* p_pool){
    thread_manager_ = const_cast<ThreadPoolManager*>(p_pool);
}


#endif
