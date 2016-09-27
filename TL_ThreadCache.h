/*
 * File:   TL_ThreadCache.h
 * Author: everpan
 *
 * Created on 2011年6月29日, 上午9:58
 */

//多线程 数据池
#ifndef TL_ThreadCache_H
#define	TL_ThreadCache_H
#include <set>
#include "TL_Singleton.h"
#include "TL_Lock.h"
#include "TL_Exp.h"
#include "TL_TSD.h"

namespace tidp {

typedef struct {
	size_t total_size;
	size_t idle_size;
	size_t busy_size;
	size_t prep_size;
} POOL_STATUS_T;

template<class T, template<class > class CreatePolicy = CreateNew>
class TL_ThreadDataPool: public TL_ThreadLock {
public:
	typedef T DATE_TYPE;

	/**
	 * get
	 * 从池中获取一个可用对象，如果没有超过池的最大，则新产生一个。
	 * @return
	 */
	TL_ThreadDataPool() {
		_max = 2000;
		busy.clear();
		idle.clear();
		_prep.clear();
	}

	~TL_ThreadDataPool() {
		//clear();
	}
	//强制清除。 可能带来一些问题。

	void clear() {
		Lock lk(*this);
		typename std::set<T*>::iterator it = busy.begin();
		while (it != busy.end()) {
			CreatePolicy<T>::destory(*it);
			++it;
		}
		it = idle.begin();
		while (it != idle.end()) {
			CreatePolicy<T>::destory(*it);
			++it;
		}
		busy.clear();
		idle.clear();
	}

	/**
	 * get
	 * 取得数据，使用，此时T*处于忙碌队列，如果达限制 将等待
	 * @return
	 */
	T* get() {
		Lock lk(*this);
		return getNoLock();
	}
	T* getNoLock() {
		while (idle.size() == 0) {
			if (busy.size() >= _max) {
				wait();
			} else {
				T* t = CreatePolicy<T>::create();
				busy.insert(t);
				return t;
			}
		}
		typename std::set<T*>::iterator it = idle.begin();
		T* t = *(it);
		idle.erase(it);
		busy.insert(t);
		notifyAll();
		return t;
	}
	/**
	 todo 未测试
	 */
	void getByNum22(T** r, int num) {
		return;
		//todo
		TL_ThreadLock::Lock lk(*this);
		_getByNum_label: if (num < 0 || num > _max) {
			r = NULL;
			throw TL_Exp("TL_ThreadDataPool::getByNum out of max num.");
		}
		if (idle.size() < num) {
			if (busy.size() >= _max) {
				wait();
			} else {
				for (int i = 0; i < num; ++i) {
					r[i] = CreatePolicy<T>::create();
					return;
				}
			}
		}
		if (idle.size() < num) {
			goto _getByNum_label;
		} else {
			typename std::set<T*>::iterator it = idle.begin();

		}
	}

	void release(const T *t) {
		TL_ThreadLock::Lock lk(*this);
		releaseNoLock(t);
	}
	void releaseNoLock(const T *t) {
		typename std::set<T*>::iterator it = busy.find(const_cast<T*>(t));
		if (it != busy.end()) {
			T* t = *it;
			busy.erase(it);
			idle.insert(t);
			notifyAll();
		} else {
			ostringstream os;
			os << "TL_ThreadDataPool::release fail,because the ptr:" << (void*) t << " is not in using.";
			throw TL_Exception(os.str(), -1);
		}
	}

	void setPoolSize(size_t len) {
		TL_ThreadLock::Lock lk(*this);
		_max = len;
		if (_max < 5)
			_max = 5;
	}

	size_t getPoolSize() {
		return _max;
	}

	size_t getIdleSize() {
		return idle.size();
	}

	size_t getBusySize() {
		return busy.size();
	}

	void getPoolStatus(POOL_STATUS_T& status) {
		status.idle_size = idle.size();
		status.busy_size = busy.size();
		status.prep_size = _prep.size();

		status.total_size = status.idle_size + status.busy_size + status.prep_size;
	}

	/**
	 * getSize
	 * 获取对象数
	 * @return
	 */
	size_t getSize() {
		//TL_ThreadLock lk(*this);
		return idle.size() + busy.size() + _prep.size();
	}
private:
	std::set<T*> idle; //空闲
	std::set<T*> busy; //忙碌，正在使用
	std::set<T*> _prep; //需要处理队列
	//std::set<T*> _doing; //正在处理的队列
	//TL_ThreadLock _prep_lock;
	size_t _max;
};

template<class T> class NewCreatePolicy {
public:
	static T * create() {
		return new T;
	}
	static void destory(T* t) {
		delete t;
	}
};
template<class T, template<class > class createPolicy = NewCreatePolicy>
class TL_ThreadSpecialData: public TL_Singleton<TL_ThreadSpecialData<T> > {
public:
	//typedef releasePolicy<T> releaseType;
	TL_ThreadSpecialData() {
		TL_Singleton<TL_ThreadSpecialData<T, createPolicy> >();
		_tsdp = new TL_TSD<Store>(my_cleanup_fun);
	}
	T * get() {
		Store * mystore = _tsdp->get();
		T * t = NULL;
		if (mystore->idle.empty()) {
			t = createPolicy<T>::create();
		} else {
			t = *(mystore->idle.begin());
			mystore->idle.erase(t);
		}
		mystore->busy.insert(t);
		return t;
	}
	void release(T * t) {
		Store * mystore = _tsdp->get();
		if (mystore->busy.find(t) == mystore->busy.end()) {
			EXCEPTION("Not in using,release failed.", -1);
		} else {
			mystore->busy.erase(t);
			mystore->idle.insert(t);
		}
	}
private:
	//TL_TSD<>;
	struct Store {
		std::set<T*> idle;
		std::set<T*> busy;
	};
	static void my_cleanup_fun(Store* data) {
		typename std::set<T*>::iterator it = data->idle.begin();
		while (it != data->idle.end()) {
			createPolicy<T>::destory(*it);
			++it;
		}
		it = data->busy.begin();
		while (it != data->busy.end()) {
			createPolicy<T>::destory(*it);
			++it;
		}
	}
	TL_TSD<Store> *_tsdp;	//(default_cleanup_fun);
};
template<class T> class ThreadSpecialDataPolicy {
public:
	static T * get() {
		return TL_ThreadSpecialData<T>::getInstance()->get();
	}
	static void release(T *t) {
		TL_ThreadSpecialData<T>::getInstance()->release(t);
	}
};
template<class T, template<class > class drawPolicy = ThreadSpecialDataPolicy> class TL_ThreadCache {
public:

	TL_ThreadCache() {
		_t = drawPolicy<T>::get();
	}
	~TL_ThreadCache() {
		drawPolicy<T>::release(_t);
	}
	T * operator->() {
		return _t;
	}
	T& operator*() {
		return *_t;
	}
private:
	T *_t;
};
}
#endif	/* TL_ThreadCache_H */

