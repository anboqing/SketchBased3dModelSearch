/*
 * =====================================================================================
 *
 *       Filename:  Thread.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月27日 20时03分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>


class Thread{
public:
    Thread();
    virtual ~Thread();
    static void *thread_func(void*);

    void start();
    void join(pthread_t tid);
    void exit();
    virtual void working()=0;

    pthread_t get_tid()const {
        return tid_;
    }

private:
    pthread_t tid_;
};


#endif
