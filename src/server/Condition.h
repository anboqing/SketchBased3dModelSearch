/*
 * =====================================================================================
 *
 *       Filename:  Condition.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月27日 22时20分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CONDITION_H_
#define CONDITION_H_


#include <pthread.h>
#include "Mutex.h"

class Condition{
public:

	Condition(MutexLock *p_lock);
	~Condition();

	void wait();
	void notify();
	void notify_all(); //谨慎使用

private:
	pthread_cond_t _cond;
	MutexLock *_p_lock;  //这里的lock要用指针
};



#endif /* CONDITION_H_ */
