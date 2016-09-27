/*
 * File:   TL_Logger.h
 * Author: everpan
 *
 * Created on 2011年6月7日, 下午4:33
 */

#ifndef TL_LOGGER_H
#define	TL_LOGGER_H
#include <iostream>
#include <ostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "TL_Singleton.h"
#include "TL_Thread.h"
#include "TL_MemString.h"
#include "TL_Datetime.h"
#include "TL_Exp.h"
namespace tidp {

class TL_LoggerCache: public TL_ThreadLock {
public:
	TL_LoggerCache();
	~TL_LoggerCache();

	TL_MemString* getCache(const std::string& filename, int type);
	void prepCache(const std::string& filename, TL_MemString* memcache, int type);
	//void releasCache(TL_MemString* memcache);
	//TL_MemString* operator(const std::string& filename);
	void flush();
	int rename(const std::string& filename);
	void init(const std::string& fileprefix, int filenum, size_t filemaxsize);
	void setPeriodInfo(const std::string& fileprefix,
			unsigned int intverval = 24 * 60 * 60,int max_size = (500 << 20) );
	std::string sysinfo();

	typedef struct logfilecache_struct {
		std::list<TL_MemString*> idle; //闲置cache
		std::set<TL_MemString*> busy; //正在使用的cache
		std::list<TL_MemString*> prep; //闲置且需要处理的cache，通常等待数据清理
		//由单独线程进行处理，写线程。
		int fd;
		size_t fsize;
		//int type;
		logfilecache_struct() {
			idle.clear();
			busy.clear();
			prep.clear();
			fd = 0;
			fsize = 0;
			//type = 1;
		}
	} LOGFILECACHE_T;
private:
	std::map<std::string, LOGFILECACHE_T> _logcache;
	std::map<std::string, LOGFILECACHE_T> _logcache2;
	//std::map<std::string,TL_MemString*> _mergercache;
	std::string _file_prefix;
	TL_MemString _merger;
	//TL_MemString _merger2;
	//int _type; // 0 - 异常 1 - 滚动日志 2 - 周期日志
	//滚动日志属性
	size_t _file_max_size;
	int _file_num;
	//周期日志属性
        std::string _time_log_file_prefix;
	std::string _time_log_format;
        int _time_log_file_num;
        size_t _time_log_file_max_size;
	unsigned int _interval;
	time_t _last_file_time;
        unsigned int _time_log_size;
        time_t _start_time;
};

class TL_LoggerStrem;
typedef TL_LoggerStrem&(*TL_LoggerStremFUNT)(TL_LoggerStrem& orig);

class TL_Logger: public TL_Singleton<TL_Logger>, public TL_Thread {
public:
	TL_Logger();
	TL_LoggerStrem getLogStream(const std::string& fname, int type);
	TL_MemString* getMemString(const std::string& fname, int type);
	std::string getTimeTips();
	void prepMemString(const std::string& fname, TL_MemString* mstr, int type);
	void run();
	void init(const std::string& logprefix, int lognum = 10, size_t maxlogsize = 200 * 1024 * 1024);
	std::string sysinfo();
        void setPeriodInfo(const std::string& fileprefix,
			unsigned int intverval = 24 * 60 * 60,int max_size = (500 << 20) );
	TL_Datetime& getDatetime();
protected:
	TL_Logger(const TL_Logger& orig);
private:
	//bool _bterminated;
	TL_LoggerCache _logcache;
	TL_Datetime _dt;
};

class TL_LoggerStrem {
public:

	enum {
                ALL_LOG = 0,
		DEBUG_LOG = 1,
		INFO_LOG = 2,
		WARNING_LOG = 3,
                ERROR_LOG = 4,
                FALT_LOG = 5,
                NO_LOG = 6
	};
	TL_LoggerStrem(const std::string& fname, TL_MemString * mstr, int type);
	//TL_LoggerStrem(const TL_LoggerStrem& orig);
	virtual ~TL_LoggerStrem();

	template<class T>
	TL_LoggerStrem& operator <<(const T& t) {
		if (_current_level >= _level) {
			if (_mstring == NULL) {
				_mstring = TL_Logger::getInstance()->getMemString(_fname, _type);
				(*_mstring) << TL_Logger::getInstance()->getTimeTips();
			}
			(*_mstring) << t;
		}
		return *this;
	}

	TL_LoggerStrem& operator <<(TL_LoggerStremFUNT fn) {
		fn(*this);
		return *this;
	}
	void checkMemString() {
		if (_mstring == NULL) {
			TL_Logger* logerPtr = TL_Logger::getInstance();
			_mstring = logerPtr->getMemString(_fname, _type);
			(*_mstring) << logerPtr->getTimeTips();
		}
	}
	bool checkLevel(){
		return (_current_level >= _level) > 0;
	}
	TL_MemString& getMemString();
	//void setMemString(TL_MemString *mstr);
	//const std::string& getFilename();

	void flush();
	void setCurrentLevel(int level) {
		_current_level = level;
                //if(_current_level == NO_LOG) ++ _current_level;
	}

	int getCurrentLevel() {
		return _current_level;
	}
	void setLevel(int level) {
		_level = level;
                if(_level == NO_LOG) ++_level;
	}

	int getLevel() {
		return _level;
	}
protected:
	//TL_LoggerStrem(const TL_LoggerStrem&);
	TL_LoggerStrem& operator =(const TL_LoggerStrem& orig);
private:
	TL_MemString * _mstring;
	std::string _fname;
	//TL_Datetime _dt;
	int _type; // 1 - 滚动日志，2-周期日志
	int _level;
	int _current_level;
};
TL_LoggerStrem& noop(TL_LoggerStrem& orig);
TL_LoggerStrem& flush(TL_LoggerStrem& orig);
TL_LoggerStrem& debug(TL_LoggerStrem& orig);
TL_LoggerStrem& info(TL_LoggerStrem& orig);
TL_LoggerStrem& error(TL_LoggerStrem& orig);
TL_LoggerStrem& warn(TL_LoggerStrem& orig);
TL_LoggerStrem& endl(TL_LoggerStrem& orig);

#define THREAD_LOG_SUPPORT TL_Logger::getInstance()->start();
//#define LOG(x) (TL_Logger::getInstance()->getLogStream("",1))
//#define LOGL(x) (TL_Logger::getInstance()->getLogStream("",1) << debug << __FILE__ << ":" << __LINE__ << ' ')

#define RLOG(x) (TL_Logger::getInstance()->getLogStream(x,1))
#define RLOGL(x) (TL_Logger::getInstance()->getLogStream(x,1) << __FILE__ << ":" << __LINE__ << ' ')
#define LOG(x) RLOGL("sys")
#define DLOG(x) (TL_Logger::getInstance()->getLogStream(x,2))
#define DLOGL(x) (TL_Logger::getInstance()->getLogStream(x,2) << debug << __FILE__ << ":" << __LINE__ << ' ')
}
#endif	/* TL_LOGGER_H */

