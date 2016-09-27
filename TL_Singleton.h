/* 
 * File:   TL_Singleton.h
 * Author: everpan
 *
 * Created on 2011年5月23日, 下午6:10
 */

#ifndef TL_SINGLETON_H
#define	TL_SINGLETON_H
#include "TL_ThreadLock.h"
namespace tidp {
    //创建形式

    template <class T>
    class CreateNew {
    public:

        static T* create() {
            return new T();
        }

        static void destory(T*t) {
            delete t;
        }
    };

    template <class T>
    class CreateStatic {
    public:

        union MaxAlign {
            char t_[sizeof (T)];
            long double longDouble_;
        };

        static T* create() {
            static MaxAlign t;
            return new(&t) T;
        }

        static void destor(T *t) {
            t->~T();
        }
    };

    template<class T, template<class> class CreatePolicy = CreateNew>
    class TL_Singleton {
    public:
        //typedef T* SINGLETON_T;
        typedef TL_Singleton<T,CreatePolicy> SINGLETON_T;
        static T* getInstance() {
            if (!_pInstance) {
                TL_ThreadMutex::Lock lk(_lock);
                if (!_pInstance) {
                    _pInstance = CreatePolicy<T>::create();
                }
            }
            return (T*) _pInstance;
        }

        static void destory() {
            if (_pInstance) {
                CreatePolicy<T>::destory(_pInstance);
                _pInstance = NULL;
                _bdestory = true;
            }
        }
    protected:
        static TL_ThreadMutex _lock;
        static volatile T* _pInstance;
        static bool _bdestory;
    };
    
     template<class T, template<class> class CreatePolicy> TL_ThreadMutex
     TL_Singleton<T,CreatePolicy>::_lock;
     
     template<class T, template<class> class CreatePolicy> volatile T*
     TL_Singleton<T,CreatePolicy>::_pInstance = NULL;
     
     template<class T, template<class> class CreatePolicy> bool
     TL_Singleton<T,CreatePolicy>::_bdestory = false;
}
#endif	/* TL_SINGLETON_H */

