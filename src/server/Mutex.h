/*
 * =====================================================================================
 *
 *       Filename:  Mutex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月27日 22时20分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  An Boqing (abq), hunkann@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef MUTEXLOCK_H_
#define MUTEXLOCK_H_


#include <pthread.h>

class Condition;

class MutexLock {
	friend class Condition;
public:
	
	MutexLock();
	~MutexLock();

	void lock();
	void unlock();

private:
	pthread_mutex_t _mutex;
};


class LockGuarde {
public:
	LockGuarde(MutexLock &l):lock_(l){
		lock_.lock();
	}
	~LockGuarde(){
		lock_.unlock();
	}
private:
	MutexLock &lock_;
};


#endif /* MUTEXLOCK_H_ */
