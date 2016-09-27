/* 
 * File:   TL_Monitor.h
 * Author: everpan
 *
 * Created on 2011年4月21日, 下午8:48
 */

#ifndef TL_MONITOR_H
#define	TL_MONITOR_H

#include "TL_Lock.h"
#include <iostream>
using namespace std;
namespace tidp {

    template <class M, class C>
    class TL_Monitor {
    public:

        TL_Monitor() : _notify(0) {
        }

        virtual ~TL_Monitor() {
        }

        typedef TL_Lock<TL_Monitor<M, C> > Lock;

        void lock() const {
            _mutex.lock();
            _notify = 0;
        }

        void unlock() const {
            notifyImpl(_notify);
            _mutex.unlock();
        }

        bool tryLock() const {
            bool result = _mutex.tryLock();
            if (result) {
                _notify = 0;
            }
            return result;
        }
        /*
         * lock 之后notify，unlock唤醒
         * */
        void notify() {
            if (_notify != -1) {
                ++_notify;
            }
        }

        void notifyAll() {
            _notify = -1;
        }

        void notifyImpl(int notify) const {
            if (notify != 0) {
                if (notify == -1) {
                    _cond.broadcast();
                } else {
                    while (notify > 0) {
                        _cond.signal();
                        --notify;
                    }
                }
            }
        }

        void wait() const {
            notifyImpl(_notify);
            try {
                _cond.wait(_mutex);
            } catch (...) {
                _notify = 0;
                throw;
            }
            _notify = 0;
        }
        
        void timedwait(int millsecond) const{
            _cond.timedwait(_mutex,millsecond);
        }
    private:
        
        TL_Monitor(const TL_Monitor<M,C> &);
        mutable volatile int _notify;
        mutable C _cond;
        mutable M _mutex;
    };

}


#endif	/* TL_MONITOR_H */

