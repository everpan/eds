/*
 * File:   TL_MemString.h
 * Author: everpan
 *
 * Created on 2011年6月7日, 下午8:30
 * modify on 2014.10.16 简单化，不做循环队列
 */
//纯粹的内存cache 类似于string，无格式。
//无锁版，多线程下需要自己额外加锁
#ifndef TL_MemString_H
#define	TL_MemString_H
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <errno.h>
#include <iostream>

#include <TL_Exp.h>
#define TL_MEMSTRING_DEFAULT_SIZE 512
namespace tidp {
class TL_MemString;
typedef TL_MemString& (*TL_MemStringFUN)(TL_MemString& orig);

class TL_MemString {
public:
	/*
	 enum {
	 eStr = 1, eBit = 2
	 };
	 */
	TL_MemString(size_t len)
			: _addr(NULL), _used_len(0), _len(TL_MEMSTRING_DEFAULT_SIZE), _isattache(false) {
		init(_len);
	}

	TL_MemString()
			: _addr(NULL), _used_len(0), _len(TL_MEMSTRING_DEFAULT_SIZE), _isattache(false) {
		init(_len);
	}

	virtual ~TL_MemString();
	int init(size_t len);
	int resize(size_t len);
	//外界内存为cache
	//void attache(char* addr, size_t len);
	bool isFull(const size_t &len);
	bool isEmpty() const;
	//bool isDestory();

	void destory();
	void clear();
	//
	//size_t reserve();
	//使用空间
	size_t size();
	size_t capacity();

	const char* c_str();
	void append(const TL_MemString& mc);
	void append(const std::string& s);

	bool push_backNT(const char* addr, size_t len); //no throw
	//void pop_frontNT(const char* addr,size_t& len);
	//写入到某个文件
	int write(int fd);

	TL_MemString& operator=(const std::string& s);

	TL_MemString & operator<<(char n);
	TL_MemString & operator<<(unsigned char n);

	TL_MemString & operator<<(short n);
	TL_MemString & operator<<(unsigned short n);

	TL_MemString & operator<<(int n);
	TL_MemString & operator<<(unsigned int n);

	TL_MemString & operator<<(long n);
	TL_MemString & operator<<(unsigned long n);

	TL_MemString & operator<<(long long n);
	TL_MemString & operator<<(unsigned long long n);

	TL_MemString & operator<<(double n);
	TL_MemString & operator<<(float n);
	//TL_MemString & operator<<(const char[] s);
	//TL_MemString & operator<<(TL_MemString& mstr,const char* s);
	TL_MemString & operator<<(const char* s);
	TL_MemString & operator<<(void * s);
	TL_MemString & operator<<(const std::string& s);

	TL_MemString& operator <<(TL_MemStringFUN fun);
protected:
	TL_MemString(const TL_MemString& orig);
	TL_MemString& operator =(const TL_MemString&);
private:
	char * _addr;
	size_t _used_len; //已使用大小
	size_t _len;
	bool _isattache;
	char _tmp[64];
};

TL_MemString& endl(TL_MemString& orig);
}
#endif	/* TL_MemString_H */

