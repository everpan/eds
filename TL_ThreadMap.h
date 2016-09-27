/* 
 * File:   TL_ThreadMap.h
 * Author: everpan
 *
 * Created on 2011年5月10日, 上午10:36
 */

#ifndef TL_THREADMAP_H
#define	TL_THREADMAP_H
#include "TL_ThreadLock.h"
#include "TL_Exp.h"
#include <map>
#include <set>
namespace tidp {

    template <class K, class V>
    class TL_ThreadMap : public TL_ThreadLock {
    public:
        typedef K key_type;

        //TL_ThreadMap(){}
        //virtual ~TL_ThreadMap(){}

        void push(const K& k, const V& v) {
            Lock lk(*this);
            _m[k] = v;
        }

        bool pop(const K& k, V& v) {
            Lock lk(*this);
            typename std::map<K, V>::iterator it = _m.find(k);
            if (it == _m.end()) {
                return false;
            } else {
                v = it->second;
                _m.erase(it);
                return true;
            }
        }
        
        size_t size(){
            return _m.size();
        }
        //不安全
        const std::map<K, V>& getMapRef() const{
            return _m;
        }
        /*
        void lock(){
            this->lock();
        }
        void unlock(){
            this->unlock();
        }*/
    private:
        std::map<K, V> _m;
    };

    template <class K>
    class TL_ThreadSet : public TL_ThreadLock {
    public:
        typedef K key_type;
        typedef typename std::set<K>::iterator iterator;

        bool find(const K& k) {
            Lock lk(*this);
            return _m.find(k) != _m.end();
        }

        void erase(const K& k) {
            Lock lk(*this);
            _m.erase(k);
        }

        size_t size() {
            return _m.size();
        }

        std::pair<iterator, bool> insert(const K&k) {
            Lock lk(*this);
            return _m.insert(k);
        }
    private:
        std::set<K> _m;
    };

    template <class K1, class K2>
    class TL_ThreadKKMap : public TL_ThreadLock {
    public:
        typedef K1 key_type;

        bool find(const K1 &k1, K2& k2) {
            typename std::map<K1, K2>::const_iterator it = _m12.find(k1);
            if (it != _m12.end()) {
                k2 = it->second;
                return true;
            }
            return false;
        }

        bool findK(const K2& k2, K1&k1) {
            typename std::map<K2, K1>::const_iterator it = _m21.find(k2);
            if (it != _m21.end()) {
                //todo
                k1 = it->second;
                return true;
            }
            return false;
        }

        void erase(const K1& k1) {
            Lock lk(*this);
            typename std::map<K1, K2>::const_iterator it = _m12.find(k1);
            if (it != _m12.end()) {
                typename std::map<K2, K1>::const_iterator it2 = _m21.find(it->second);
                _m21.erase(it2);
                _m12.erase(it);
            }
        }

        void insert(const K1& k1, const K2& k2) {
            Lock lk(*this);
            _m12[k1] = k2;
            _m21[k2] = k1;
        }
        
        size_t size(){
            return _m12.size();
        }
    private:
        std::map<K1, K2> _m12;
        std::map<K2, K1> _m21;
    };

}
#endif	/* TL_THREADMAP_H */

