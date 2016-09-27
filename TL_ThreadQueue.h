/*
 * File:   TL_ThreadQueue.h
 * Author: everpan
 *
 * Created on 2011年4月25日, 下午2:51
 */

#ifndef TL_THREADQUEUE_H
#define	TL_THREADQUEUE_H
#include <deque>
#include <map>
#include "TL_ThreadLock.h"
namespace tidp {

template<class T, class D = std::deque<T> >
class TL_ThreadQueue: public TL_ThreadLock {
public:

	TL_ThreadQueue() {
		_size = 0;
	}
public:

	typedef D queue_type;

	bool pop_front(T& t, int millsecond = -1) {
		wait_loop: Lock lk(*this);

		if (_queue.empty()) {
			if (millsecond == 0) {
				return false;
			} else if (millsecond == -1) {
				wait();
			} else {
				if (millsecond < 0) {
					millsecond = -millsecond;
				}
				timedwait(millsecond);
				if (_queue.empty()) {
					return false;
				}
			}
		}
		//wake and the queue also empty
		if (_queue.empty()) {
			//modify @ 20111203 everpan
			lk.release();
			goto wait_loop;
		}
		t = _queue.front();
		_queue.pop_front();
		--_size;
		return true;
	}

	void push_back(const T& t) {
		Lock lk(*this);
		_queue.push_back(t);
		notifyAll();
		++_size;
	}

	void push_back(const queue_type &qt) {
		Lock lk(*this);
		typename queue_type::const_iterator it = qt.begin();
		typename queue_type::const_iterator it_end = qt.end();
		while (it != it_end) {
			_queue.push_back(*it);
			++it;
		}
		_size += qt.size();
		notifyAll();
	}

	void push_front(const T& t) {
		Lock lk(*this);
		notifyAll();
		++_size;
		_queue.push_front(t);
	}

	void push_front(const queue_type &qt) {
		Lock lk(*this);
		typename queue_type::const_iterator it = qt.begin();
		typename queue_type::const_iterator it_end = qt.end();
		while (it != it_end) {
			_queue.push_front(*it);
			++it;
		}
		_size += qt.size();
		notifyAll();
	}

	bool swap(queue_type &q, size_t millsecond = 0);

	size_t size() const {
		Lock lk(*this);
		return _queue.size();
	}

	size_t size2() const {
		return _size;
	}

	void clear() {
		Lock lk(*this);
		_queue.clear();
	}

	bool empty() const {
		Lock lk(*this);
		return _queue.empty();
	}

	/*void notifyAll() {
	 Lock lk(*this);
	 notifyAll();
	 }*/
protected:

	queue_type _queue;
	size_t _size;
	//TL_ThreadLock _lock;
};
}

#endif	/* TL_THREADQUEUE_H */

