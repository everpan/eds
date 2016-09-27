/* 
 * File:   TL_Thread.h
 * Author: everpan
 *
 * Created on 2011年4月26日, 上午10:02
 */

#ifndef TL_THREAD_H
#define	TL_THREAD_H

#include <pthread.h>
#include <signal.h>
#include <vector>
#include <list>
#include <set>
#include <unistd.h>
#include <sys/types.h>

#include "TL_ThreadLock.h"
#include "TL_ThreadQueue.h"
#include "TL_Exp.h"
namespace tidp {
class TL_ThreadControl {
public:
	TL_ThreadControl();
	explicit TL_ThreadControl(pthread_t pid);
	void detach();
	void join();
	pthread_t id();
	static void yield();
private:
	pthread_t _threadid;
};

class TL_Thread {
public:
	TL_Thread();

	virtual ~TL_Thread();
	virtual void run() = 0;

	pthread_t id();
	bool isRunning() const;
	bool needJoin() const;
	TL_ThreadControl start();
	void stop();
	void static threadEntry(TL_Thread * pThread);
	TL_ThreadControl getControl() const;
	TL_ThreadLock& getThreadLockRef();
	pid_t getTid();
protected:
	TL_Thread(const TL_Thread& orig);
	char _running;
	pthread_t _threadid;
	TL_ThreadLock _lock;
	//pid_t _tid;
	// void *_loger;
};

class TL_Runable {
public:
	virtual void operator()() = 0;

	virtual ~TL_Runable() {
	}
	;
};
//class TL_ThreadPool;

class TL_ThreadPool: public TL_ThreadLock {
public:
	TL_ThreadPool();
	virtual ~TL_ThreadPool();
	void init(size_t num);
	size_t getThreadNum();
	void start();
	void stop();
	void clear();
	bool finish();
	void addJob(TL_Runable* job);
	void addRunable(TL_Runable* rable);
	void addJobs(const TL_ThreadQueue<TL_Runable*>::queue_type& q);

	bool finished();
	void waitingAllDone();

	//void idle();

	class TL_ThreadWorker: public TL_Thread {
	public:
		TL_ThreadWorker(TL_ThreadPool* pool);

		~TL_ThreadWorker() {
		}

		void terminate();

		virtual void run();

	private:
		bool _bterminate;

		TL_ThreadPool* _threadpool;
	};
private:
	std::vector<TL_ThreadWorker*> _jobthread;
	std::set<TL_ThreadWorker*> _usingthread;
	TL_ThreadQueue<TL_Runable*> _jobqueue;
	int _threadnum;
	//TL_ThreadLock _lock;
	friend class TL_ThreadWorker;
};
}
#endif	/* TL_THREAD_H */

