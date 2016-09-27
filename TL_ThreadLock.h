/* 
 * File:   TL_ThreadLock.h
 * Author: everpan
 *
 * Created on 2011年3月18日, 下午3:59
 */

#ifndef TL_THREADLOCK_H
#define	TL_THREADLOCK_H
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include "TL_Exp.h"
#include "TL_Monitor.h"

namespace tidp {

class TL_ThreadMutex {
public:
	typedef TL_Lock<TL_ThreadMutex> Lock;
	TL_ThreadMutex();
	virtual ~TL_ThreadMutex();

	void lock() const;
	void unlock() const;
	bool tryLock() const;
	friend class TL_ThreadCond;
protected:
	TL_ThreadMutex(const TL_ThreadMutex&);
	mutable pthread_mutex_t _mutex;
};

class TL_ThreadCond {
public:
	TL_ThreadCond();
	virtual ~TL_ThreadCond();

	bool wait(const TL_ThreadMutex& mutex);
	bool timedwait(const TL_ThreadMutex& mutex, int millsecond);
	static struct timespec abstime(int millsecond);
	void broadcast();
	void signal();
protected:
	TL_ThreadCond(const TL_ThreadCond&);
	mutable pthread_cond_t _cond;
};

class TL_ThreadRwLock {
public:
	typedef TL_RdLock<TL_ThreadRwLock> RLock;
	typedef TL_WrLock<TL_ThreadRwLock> WLock;
	TL_ThreadRwLock();
	~TL_ThreadRwLock();
	int rdlock() const;
	int wrlock() const;
	int unlock() const;
protected:
	TL_ThreadRwLock(const TL_ThreadRwLock&);
	mutable pthread_rwlock_t _rwlock;
};
typedef TL_Monitor<TL_ThreadMutex, TL_ThreadCond> TL_ThreadLock;
typedef TL_Lock<TL_ThreadMutex> TL_MutexLock;
//typedef TL_Lock<TL_ThreadMutex> TL_ThreadMutexLock;
}
#endif	/* TL_THREADLOCK_H */

