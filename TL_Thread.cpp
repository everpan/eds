/* 
 * File:   TL_Thread.cpp
 * Author: everpan
 * 
 * Created on 2011年4月26日, 上午10:02
 */

#include "TL_Thread.h"
namespace tidp {

TL_ThreadControl::TL_ThreadControl() :
		_threadid(pthread_self()) {
}

TL_ThreadControl::TL_ThreadControl(pthread_t pid) :
		_threadid(pid) {
}

void TL_ThreadControl::join() {
	if (pthread_equal(pthread_self(), _threadid)) {
		throw TL_Exp("[TL_ThreadControl::join] can't be called in the same thread.");
	}
	//void* ignore = 0;
	int rc = pthread_join(_threadid, NULL);
	if (rc != 0) {
		throw TL_Exp("[TL_ThreadControl::join] pthread_join error.", rc);
	}
}

void TL_ThreadControl::detach() {
	if (pthread_equal(pthread_self(), _threadid)) {
		throw TL_Exp("[TL_ThreadControl::detach] can't be called in the same thread.");
	}
	int rc = pthread_detach(_threadid);
	if (rc != 0) {
		throw TL_Exp("[TL_ThreadControl::detach] pthread_join error.", rc);
	}
}

pthread_t TL_ThreadControl::id() {
	return _threadid;
}

void TL_ThreadControl::yield() {
	sched_yield();
}

TL_Thread::TL_Thread() :
		_running(0) {
	//_loger = NULL;
	_threadid = pthread_self();
	//_tid = gettid();
}

TL_Thread::~TL_Thread() {
	_running = 0;
}

pthread_t TL_Thread::id() {
	return _threadid;
}

void TL_Thread::threadEntry(TL_Thread * pThread) {
	pThread->_running = 1;
	{
		TL_ThreadLock::Lock lock(pThread->_lock);
		//20150106 通知创建者，本君已经就绪，并打算执行。
		pThread->_lock.notify();//All();
	}

	try {
		pThread->run();
	} catch (TL_Exp &e) {
		pThread->_running = 2;
		throw e;
	} catch (...) {
		pThread->_running = 2;
		throw TL_Exp("[TL_Thread::run()] exception.");
	}
	pThread->_running = 2; // can join
}

TL_ThreadControl TL_Thread::start() {
	TL_ThreadLock::Lock lock(_lock);
	if (_running) {
		throw TL_Exception("[TL_Thread::start] thread has started.",-1);
	}
	int ret = pthread_create(&_threadid, 0, (void *(*)(void*)) & threadEntry, (void*) this);

	if (ret != 0) {
		throw TL_Exception("[TL_Thread::start] thread start error", ret);
	}
	//20140810 ever 这里wait的意义是？忘记了
	_lock.wait();
	//20150105 ever 等待子线程创建ok 并通知，确保返回后子线程是已经创建的
	return TL_ThreadControl(_threadid);
}
void TL_Thread::stop() {
	_running = 0;
}
bool TL_Thread::isRunning() const {
	return _running == 1;
}

bool TL_Thread::needJoin() const {
	return _running == 2;
}

TL_ThreadControl TL_Thread::getControl() const {
	return TL_ThreadControl(_threadid);
}
TL_ThreadLock& TL_Thread::getThreadLockRef() {
	return _lock;
}

////////////////////////////////////////////////////////////////////////////

TL_ThreadPool::TL_ThreadWorker::TL_ThreadWorker(TL_ThreadPool* pool) :
		_bterminate(false), _threadpool(pool) {
}

void TL_ThreadPool::TL_ThreadWorker::terminate() {
	_bterminate = true;
}

void TL_ThreadPool::TL_ThreadWorker::run() {
	TL_Runable* r = NULL;
	while (!_bterminate) {
		_threadpool->_jobqueue.pop_front(r);
		if (r) {
			try {
				(*r)();
				//todo runable的销毁？？？
			} catch (...) {
			}
		}
	}
	//exit;
	{
		TL_ThreadLock::Lock lk(*_threadpool);
		--_threadpool->_threadnum;
		if (_threadpool->_threadnum == 0) {
			_threadpool->notify();
		}
	}
}

TL_ThreadPool::TL_ThreadPool() :
		_threadnum(0) {
}

TL_ThreadPool::~TL_ThreadPool() {
	stop();
	clear();
}

void TL_ThreadPool::init(size_t num) {
	Lock lk(*this);
	_threadnum = num;
	for (size_t i = 0; i < num; ++i) {
		_jobthread.push_back(new TL_ThreadWorker(this));
	}
}

void TL_ThreadPool::clear() {
	Lock lk(*this);
	std::vector<TL_ThreadPool::TL_ThreadWorker*>::iterator it = _jobthread.begin();
	while (it != _jobthread.end()) {
		delete (*it);
		++it;
	}
	_jobthread.clear();
	_usingthread.clear();
}

size_t TL_ThreadPool::getThreadNum() {
	return _threadnum;
}

void TL_ThreadPool::start() {
	Lock lk(*this);
	std::vector<TL_ThreadPool::TL_ThreadWorker*>::iterator it = _jobthread.begin();
	while (it != _jobthread.end()) {
		(*it)->start();
		++it;
	}
}

void TL_ThreadPool::stop() {
	//当前任务执行完就退出,不等待队列空
	Lock lk(*this);
	std::vector<TL_ThreadPool::TL_ThreadWorker*>::iterator it = _jobthread.begin();
	while (it != _jobthread.end()) {
		if ((*it)->isRunning()) {
			(*it)->terminate();
		}
		++it;
	}
	_jobqueue.notifyAll();
	wait();
	it = _jobthread.begin();
	while (it != _jobthread.end()) {
		if ((*it)->needJoin()) {
			(*it)->getControl().join();
		}
		++it;
	}
}

bool TL_ThreadPool::finished() {
	return _jobqueue.empty();
}
void TL_ThreadPool::waitingAllDone() {
	if (finished()) {
		stop();
	} else {
		//等待退出
		wait();
	}
}
void TL_ThreadPool::addJob(TL_Runable* job) {
	_jobqueue.push_back(job);
}
void TL_ThreadPool::addRunable(TL_Runable* rable) {
	_jobqueue.push_back(rable);
}
void TL_ThreadPool::addJobs(const TL_ThreadQueue<TL_Runable*>::queue_type& q) {
	_jobqueue.push_back(q);
}
}

