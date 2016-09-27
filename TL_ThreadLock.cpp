/* 
 * File:   TL_ThreadLock.cpp
 * Author: everpan
 * 
 * Created on 2011年3月18日, 下午3:59
 */

#include "TL_ThreadLock.h"
namespace tidp {

TL_ThreadMutex::TL_ThreadMutex() {
	int rc = pthread_mutex_init(&_mutex, NULL);
	if (rc != 0) {
		throw TL_Exp("TL_ThreadMutex pthread_mutex_init error.", rc);
	}
}

TL_ThreadMutex::~TL_ThreadMutex() {
	int rc = pthread_mutex_destroy(&_mutex);
	if (rc != 0) {
		throw TL_Exp("TL_ThreadMutex pthread_mutex_destroy error.", rc);
	}
}

void TL_ThreadMutex::lock() const {
	int rc = pthread_mutex_lock(&_mutex);
	if (rc != 0) {
		if (rc == EDEADLK) {
			throw TL_Exception("[TL_ThreadMutex::lock] pthread_mutex_lock dead lock error.", rc);
		} else {
			throw TL_Exception("[TL_ThreadMutex::lock] pthread_mutex_lock error.", rc);
		}
	}
}

bool TL_ThreadMutex::tryLock() const {
	int rc = pthread_mutex_trylock(&_mutex);
	if (rc != 0 && rc != EBUSY) {
		if (rc == EDEADLK) {
			throw TL_Exception("[TL_ThreadMutex::tryLock] pthread_mutex_trylock dead lock error.", rc);
		} else {
			throw TL_Exception("[TL_ThreadMutex::tryLock] pthread_mutex_trylock error.", rc);
		}
	}
	return (rc == 0);
}

void TL_ThreadMutex::unlock() const {
	int rc = pthread_mutex_unlock(&_mutex);
	if (rc != 0) {
		throw TL_Exception("[TL_ThreadMutex::unlock] pthread_mutex_unlock error.", rc);
	}
}
//--------------------- TL_ThreadCond -------------------------------------------

TL_ThreadCond::TL_ThreadCond() {
	int rc = pthread_cond_init(&_cond, NULL);
	if (rc != 0) {
		throw TL_Exception("[TL_ThreadCond::TL_ThreadCond] pthread_cond_init error", rc);
	}
}

TL_ThreadCond::~TL_ThreadCond() {
	int rc = pthread_cond_destroy(&_cond);
	if (rc != 0) {
		throw TL_Exception("[TL_ThreadCond::~TL_ThreadCond] pthread_cond_destroy error", rc);
	}
}

void TL_ThreadCond::signal() {
	int rc = pthread_cond_signal(&_cond);
	if (rc != 0) {
		throw TL_Exception("[TL_ThreadCond::signal] pthread_cond_signal error.", errno);
	}
}

void TL_ThreadCond::broadcast() {
	int rc = pthread_cond_broadcast(&_cond);
	if (rc != 0) {
		throw TL_Exception("[TL_ThreadCond::broadcast] pthread_cond_broadcast error.", errno);
	}
}

bool TL_ThreadCond::wait(const TL_ThreadMutex& mutex) {
	int rc = pthread_cond_wait(&_cond, &mutex._mutex);
	if (rc != 0) {
		throw TL_Exception("[TC_ThreadCond::wait] pthread_cond_wait error", errno);
	}
	return true;
}

bool TL_ThreadCond::timedwait(const TL_ThreadMutex& mutex, int millsecond) {
	timespec ts = abstime(millsecond);
	int rc = pthread_cond_timedwait(&_cond, &mutex._mutex, &ts);
	if (rc != 0) {
		if (rc != ETIMEDOUT) {
			throw TL_Exception("[TL_ThreadCond::timedwait] pthread_cond_timedwait error", errno);
		}

		return false;
	}
	return true;
	throw TL_Exception("TL_ThreadCond::timedwait hasn't implememtion.", -1);
	return false;
}

struct timespec TL_ThreadCond::abstime(int millsecond) {
	struct timeval tv;
	//time_t t = time(NULL);
	gettimeofday(&tv, NULL);
	int64_t it = tv.tv_sec * (int64_t) 1000000 + tv.tv_usec + millsecond * 1000;

	tv.tv_sec = it / (int64_t) 1000000;
	tv.tv_usec = it % (int64_t) 1000000;

	timespec ts;
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;

	return ts;
}

TL_ThreadRwLock::TL_ThreadRwLock() {
	int rc = pthread_rwlock_init(&_rwlock, NULL);
	if (rc != 0) {
		throw TL_Exception("[TL_ThreadRwLock::TL_ThreadRwLock] pthread_rwlock_init error.", errno);
	}
}

TL_ThreadRwLock::~TL_ThreadRwLock() {
	int rc = pthread_rwlock_destroy(&_rwlock);
	if (rc != 0) {
		throw TL_Exception("[TL_ThreadRwLock::~TL_ThreadRwLock] pthread_rwlock_destroy error.", errno);
	}
}

int TL_ThreadRwLock::rdlock() const {
	return pthread_rwlock_rdlock(&_rwlock);
}

int TL_ThreadRwLock::wrlock() const {
	return pthread_rwlock_wrlock(&_rwlock);
}

int TL_ThreadRwLock::unlock() const {
	return pthread_rwlock_unlock(&_rwlock);
}
}
