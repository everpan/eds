/*
 * File:   TL_MemString.cpp
 * Author: everpan
 *
 * Created on 2011年6月7日, 下午8:30
 */

#include "TL_MemString.h"
namespace tidp {

TL_MemString::~TL_MemString() {

}

int TL_MemString::init(size_t len) {
	if (len < 1)
		return 0;
	_addr = (char*) malloc(len);
	_len = len;
	return len;
}

int TL_MemString::resize(size_t len) {
	//外部的内存 不能扩展。
	if (_isattache) {
		return 0;
	}
	if (_len == 0) {
		init(TL_MEMSTRING_DEFAULT_SIZE);
	}
	if (len > _len) {
		char *newaddr = (char *) realloc(_addr, len);
		if (newaddr == NULL) {
			destory();
			throw TL_Exception("TL_MemString realloc failed", errno);
		}
		_addr = newaddr;
		_len = len;
	}
	return _len;
	/*
	 if (len > _len) {
	 //放大
	 char * newaddr = (char*) malloc(len);
	 if (_top < _bot) {
	 memcpy(newaddr, _addr + _top, _bot - _top);
	 _top = 0;
	 _bot -= _top;
	 } else if (_top > _bot) {
	 size_t size = _len - _top;
	 memcpy(newaddr, _addr + _top, size);
	 memcpy(newaddr + size, _addr, _bot);
	 _top = 0;
	 _bot += size;
	 } else {
	 _top = _bot = 0;
	 }
	 destory();
	 _addr = newaddr;
	 _len = len;
	 } else if (len < _len) {
	 //todo 缩小
	 }
	 return len;
	 */
}
/*
 void TL_MemString::attache(char* addr, size_t len) {
 destory();
 _addr = addr;
 _len = len;
 _top = _bot = 0;
 _isattache = true;
 }
 */

bool TL_MemString::isFull(const size_t &len) {
	return _used_len == _len;
}

bool TL_MemString::isEmpty() const {
	return _used_len == 0;
}

void TL_MemString::destory() {
	if (!_isattache && _addr != NULL) {
		free(_addr);
		_addr = NULL;
		_len = 0;
		_used_len = 0;
	}
}

void TL_MemString::clear() {
	_used_len = 0;
}

size_t TL_MemString::size() {
	return _used_len;
}

size_t TL_MemString::capacity() {
	return _len;
}
const char* TL_MemString::c_str() {
	return _addr;
}

void TL_MemString::append(const TL_MemString& mc) {
	if (!mc.isEmpty()) {
		push_backNT(mc._addr, mc._used_len);
	}
}

/*
 void TL_MemString::setSplitChar(char c) {
 _split_char = c;
 }*/

void TL_MemString::append(const std::string& s) {
	push_backNT(s.c_str(), s.size());
}

bool TL_MemString::push_backNT(const char* addr, size_t len) {
	if (len > _len - _used_len) {
		resize(((_len + len) / TL_MEMSTRING_DEFAULT_SIZE + 1) * TL_MEMSTRING_DEFAULT_SIZE);
	}
	memcpy(_addr + _used_len, addr, len);
	_used_len += len;
	return true;
	/*
	 if (len < 1)
	 return true;
	 if (_bot >= _top) {
	 if (_len - _bot >= len) {
	 memcpy(_addr + _bot, addr, len);
	 _bot += len;
	 } else if (_len - _bot + _top > len) {
	 memcpy(_addr + _bot, addr, _len - _bot);
	 _bot = len - _len + _bot;
	 memcpy(_addr, addr + _len - _bot, _bot);
	 } else {
	 //空间不够，2倍扩展 一直扩展就有问题，需要业务自己处理
	 resize(_len * 2);
	 push_backNT(addr, len);
	 }
	 } else {
	 if (_top - _bot >= len) {
	 memcpy(_addr + _bot, addr, len);
	 _bot += len;
	 } else {
	 if (_len > 20480) {
	 resize(_len + 2048);
	 } else {
	 resize(_len * 2);
	 }
	 push_backNT(addr, len);
	 }
	 }
	 return true;
	 */
}
//no throw & no check
/*
 void TL_MemString::pop_frontNT(const char* addr, size_t& len) {

 }*/

/**
 *  内存写文件，并重置缓存
 * @param fd
 */
int TL_MemString::write(int fd) {
	size_t slen = 0, x = 0;
	while (slen < _used_len) {
		x = ::write(fd, _addr + slen, _used_len - slen);
		if (x < 0) {
			if (errno == EAGAIN) {
				continue;
			} else {
				throw TL_Exception("write fail", errno);
			}
		}
		slen += x;
	}
	return slen;
}

TL_MemString & TL_MemString::operator=(const std::string& s) {
	clear();
	push_backNT(s.c_str(), s.size());
	return *this;
}

TL_MemString & TL_MemString::operator<<(char n) {
	push_backNT(&n, 1);
	return *this;
}

TL_MemString & TL_MemString::operator<<(unsigned char n) {
	int i = sprintf(_tmp, "%d", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(short n) {
	int i = sprintf(_tmp, "%d", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(unsigned short n) {
	int i = sprintf(_tmp, "%u", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(int n) {
	int i = sprintf(_tmp, "%d", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(unsigned int n) {
	int i = sprintf(_tmp, "%u", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(long n) {
	int i = sprintf(_tmp, "%ld", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(unsigned long n) {
	int i = sprintf(_tmp, "%lu", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(long long n) {
	int i = sprintf(_tmp, "%lld", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(unsigned long long n) {
	int i = sprintf(_tmp, "%llu", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(double n) {
	int i = sprintf(_tmp, "%f", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(float n) {
	int i = sprintf(_tmp, "%f", n);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(const char* s) {
	push_backNT(s, strlen(s));
	return *this;
}
TL_MemString & TL_MemString::operator<<(void * s) {
	int i = sprintf(_tmp, "%llux", (unsigned long long)s);
	push_backNT(_tmp, i);
	return *this;
}

TL_MemString & TL_MemString::operator<<(const std::string& s) {
	push_backNT(s.c_str(), s.size());
	return *this;
}

TL_MemString& TL_MemString::operator<<(TL_MemStringFUN fun) {
	fun(*this);
	return *this;
}

TL_MemString& endl(TL_MemString& orig) {
	orig << "\n";
	return orig;
}
}
