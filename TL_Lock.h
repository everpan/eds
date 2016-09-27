/* 
 * File:   TL_Lock.h
 * Author: everpan
 *
 * Created on 2011年3月23日, 下午3:40
 */

#ifndef TL_LOCK_H
#define	TL_LOCK_H

#include "TL_Exp.h"
#include <iostream>
using namespace std;
namespace tidp {

    template <class T>
    class TL_Lock {
    public:

        TL_Lock(const T & lock) : _lock(lock) {
            _lock.lock();
            _acquired = true;
        }

        virtual ~TL_Lock() {
            if (_acquired) {
                _lock.unlock();
            }
        }
        void wait() const{
        	_lock.wait();
        }
        void release() {
            if (!_acquired) {
                throw TL_Exp("thread has't been looked!");
            }
            _lock.unlock();
            _acquired = false;
        }
    protected:
        const T & _lock;
        bool _acquired;
    };

    template <class T>
    class TL_RdLock {
    public:

        TL_RdLock(const T & lock) : _lock(lock) {
            _lock.rdlock();
            _acquired = true;
        }

        virtual ~TL_RdLock() {
            if (_acquired) {
                _lock.unlock();
            }
        }

        void release() const {
            if (!_acquired) {
                throw TL_Exp("thread has't been rlooked!");
            }
            _lock.unlock();
            _acquired = false;
        }
    protected:
        const T& _lock;
        bool _acquired;
    };

    template <class T>
    class TL_WrLock {
    public:

        TL_WrLock(const T & lock) : _lock(lock) {
            _lock.wrlock();
            _acquired = true;
        }

        virtual ~TL_WrLock() {
            if (_acquired) {
                _lock.unlock();
            }
        }

        void release() const {
            if (!_acquired) {
                throw TL_Exp("thread has't been wlooked!");
            }
            _lock.unlock();
            _acquired = false;
        }
    protected:
        const T& _lock;
        bool _acquired;
    };
}


#endif	/* TL_LOCK_H */

