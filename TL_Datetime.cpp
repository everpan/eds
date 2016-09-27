/* 
 * File:   TL_Datetime.cpp
 * Author: everpan
 * 
 * Created on 2011年3月28日, 上午11:42
 */

#include "TL_Datetime.h"
namespace tidp {
const char TL_Datetime::MON_MAX_DAY[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const char TL_Datetime::LOCALTIME_MON1[12] = { 1, -1, 0, 0, 1, 1, 2, 3, 3, 4, 4, 5 };
const char TL_Datetime::LOCALTIME_MON2[12] = { 1, 0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6 };

TL_Datetime::TL_Datetime() {
	gettimeofday(&_tv, NULL);
	_interval = 60;
	_tm_fmt = "%Y-%m-%d %H:%M:%S";
}

TL_Datetime::TL_Datetime(time_t t) {
	_tv.tv_sec = t;
	_tv.tv_usec = 0;
	_interval = 60;
	_tm_fmt = "%Y-%m-%d %H:%M:%S";
}

TL_Datetime::TL_Datetime(const struct timeval& tv) {
	_tv.tv_sec = tv.tv_sec;
	_tv.tv_usec = tv.tv_usec;
	_interval = 60;
	_tm_fmt = "%Y-%m-%d %H:%M:%S";
}

TL_Datetime::TL_Datetime(const TL_Datetime& orig) {
	_tv.tv_sec = orig._tv.tv_sec;
	_tv.tv_usec = orig._tv.tv_usec;
	_interval = orig._interval;
	_tm_fmt = orig._tm_fmt;
}

TL_Datetime::TL_Datetime(const std::string &stime, const std::string &sformat) {
	_tv.tv_sec = parse(stime, sformat);
	_tv.tv_usec = 0;
	_interval = 60;
	_tm_fmt = sformat;
}

struct timeval TL_Datetime::getTimeOfDay() {
	gettimeofday(&_tv, NULL);
	return _tv;
}

void TL_Datetime::sync() {
	gettimeofday(&_tv, NULL);
}
void TL_Datetime::syncOnlySecond() {
	//gettimeofday(&_tv, NULL);
	time(&_tv.tv_sec);
}
const struct timeval& TL_Datetime::getTimeval() {
	return _tv;
}

time_t TL_Datetime::getTimestamp() {
	return _tv.tv_sec;
}
time_t TL_Datetime::getBeijingTimestamp() {
	return _tv.tv_sec + BEIJING_TIME_INTERVAL;
}

std::string TL_Datetime::tm2str(const time_t &t, const std::string &fmt) {
	struct tm tt;
	localtime_r(&t, &tt);
	char sTimeString[120] = "\0";
	strftime(sTimeString, 100, fmt.c_str(), &tt);
	return std::string(sTimeString);
}

time_t TL_Datetime::parse(const std::string &stime, const std::string &sformat) {
	if (stime.empty())
		return 0;
	struct tm stTm;
	memset(&stTm, 0, sizeof(stTm));
	stTm.tm_mday = 1;
	char *p = strptime(stime.c_str(), sformat.c_str(), &stTm);
	if (p == NULL) {
		throw TL_Exp("strptime error:can't convert " + stime + "[" + sformat + "] to struct tm!");
	}
	time_t t = mktime(&stTm);
	if (t == (time_t) (-1)) {
		throw TL_Exp("mktime error!");
	}
	return t;
}

bool TL_Datetime::is_leap_year(int year) {
	return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

void TL_Datetime::localtime(time_t t, struct tm& ptm, int timez) {
	time_t tt = t + timez * 3600;
	register time_t daytime = tt % 86400;
	ptm.tm_hour = daytime / 3600;
	ptm.tm_min = (daytime / 60) % 60;
	ptm.tm_sec = daytime % 60;

	daytime = tt / 86400;
	ptm.tm_wday = (daytime + 4) % 7;

	int leapyear_num = daytime / 1461;
	int cycleday = daytime % 1461;
	if (cycleday == 1095) {
		ptm.tm_year = (daytime - leapyear_num - 1) / 365;
		ptm.tm_yday = 366; //(daytime - leapyear_num) % 365 + 1;
	} else if (cycleday > 1095) {
		++leapyear_num;
		ptm.tm_year = (daytime - leapyear_num) / 365;
		ptm.tm_yday = (daytime - leapyear_num) % 365 + 1;
	} else {
		ptm.tm_year = (daytime - leapyear_num) / 365;
		ptm.tm_yday = (daytime - leapyear_num) % 365 + 1;
	}

	ptm.tm_mday = ptm.tm_yday % 30;
	ptm.tm_mon = ptm.tm_yday / 30;
	//std::cout << ptm.tm_yday << "|" << ptm.tm_mon << "|" << ptm.tm_mday << std::endl;
	const char* LOCALTIME_MON = LOCALTIME_MON1;
	bool bleapyear = ((ptm.tm_year & 0x3) == 2); //is_leap_year(ptm.tm_year + 1970);
	if (bleapyear) {
		LOCALTIME_MON = LOCALTIME_MON2;
	}
	if (ptm.tm_mon > 0) {
		--ptm.tm_mon;
		if (ptm.tm_mday > LOCALTIME_MON[ptm.tm_mon]) {
			ptm.tm_mday -= LOCALTIME_MON[ptm.tm_mon];
			++ptm.tm_mon;
		} else {
			//<=
			if (bleapyear && ptm.tm_mon == 1) {
				ptm.tm_mday = 29;
			} else {
				ptm.tm_mday += MON_MAX_DAY[ptm.tm_mon] - LOCALTIME_MON[ptm.tm_mon];
			}
		}
	}
	//std::cout << ptm.tm_wday << "|" << ptm.tm_year + 1970 << "-" << ptm.tm_mon + 1 << "-"
	//       << ptm.tm_mday << " " << ptm.tm_hour + 8 << ":" << ptm.tm_min << ":" << ptm.tm_sec << std::endl;
}

void TL_Datetime::setTimeStringFormat(const std::string & fmt) {
	_tm_fmt = fmt;
}

std::string TL_Datetime::toString() const {
	return TL_Datetime::tm2str(_tv.tv_sec, _tm_fmt);
}

std::string TL_Datetime::toString(const std::string &fmt) const {
	return TL_Datetime::tm2str(_tv.tv_sec, fmt);
}

void TL_Datetime::setCronTime(char * c) {
	struct tm tt;
	localtime_r(&_tv.tv_sec, &tt);
	c[0] = tt.tm_min;
	c[1] = tt.tm_hour;
	c[2] = tt.tm_mday;
	c[3] = tt.tm_mon + 1; //1-12
	c[4] = tt.tm_wday; //0-6
}

TL_Datetime::~TL_Datetime() {
}

void TL_Datetime::setTime(const time_t& t) {
	_tv.tv_sec = t;
}
void TL_Datetime::setTime(const struct timeval & tv) {
	_tv.tv_sec = tv.tv_sec;
	_tv.tv_usec = tv.tv_usec;
}

void TL_Datetime::setInterval(int v) {
	_interval = v;
}

int TL_Datetime::getInterval() {
	return _interval;
}

void TL_Datetime::addMonth(int mon) {
	struct tm tt;
	localtime_r(&_tv.tv_sec, &tt);
	int month = tt.tm_year * 12 + tt.tm_mon;
	month += mon;
	tt.tm_year = month / 12;
	tt.tm_mon = month % 12;

	if (tt.tm_mon == 1 && tt.tm_mday > 28) {
		if (TL_Datetime::is_leap_year(tt.tm_year + 1900)) {
			tt.tm_mday = 29;
		} else {
			tt.tm_mday = 28;
		}
	} else if (tt.tm_mday > MON_MAX_DAY[tt.tm_mon]) {
		tt.tm_mday = MON_MAX_DAY[tt.tm_mon];
	}

	_tv.tv_sec = mktime(&tt); //最后更新，因为前面tt.tm_mday可能被修改。

	if (_tv.tv_sec == (time_t) (-1)) {
		throw TL_Exp("TL_Datetime::addMonth:mktime error.");
	}

}

TL_Datetime TL_Datetime::operator ++(int) {
	TL_Datetime d(*this);
	_tv.tv_sec += _interval;
	return d;
}

TL_Datetime& TL_Datetime::operator ++() {
	_tv.tv_sec += _interval;
	return *this;
}

TL_Datetime TL_Datetime::operator +(int v) {
	TL_Datetime d(*this);
	d._tv.tv_sec += (v * _interval);
	return d;
}

TL_Datetime& TL_Datetime::operator +=(int v) {
	this->_tv.tv_sec += (v * _interval);
	return *this;
}

TL_Datetime TL_Datetime::operator --(int) {
	TL_Datetime d(*this);
	_tv.tv_sec -= _interval;
	return d;
}

TL_Datetime& TL_Datetime::operator --() {
	_tv.tv_sec -= _interval;
	return *this;
}

TL_Datetime TL_Datetime::operator -(int v) {
	TL_Datetime d(*this);
	d._tv.tv_sec -= (v * _interval);
	return d;
}

TL_Datetime& TL_Datetime::operator -=(int v) {
	this->_tv.tv_sec -= (v * _interval);
	return *this;
}

/*
 struct timeval TL_Datetime::operator -(const TL_Datetime& d)
 {
 struct timeval tv;
 tv.tv_usec = _tv.tv_usec - d._tv.tv_usec;
 if (tv.tv_usec < 0)
 {
 tv.tv_sec = _tv.tv_sec - d._tv.tv_sec - 1;
 tv.tv_usec = 1000000 + tv.tv_usec;
 } else
 {
 tv.tv_sec = _tv.tv_sec - d._tv.tv_sec;
 }
 return tv;
 }
 */
const bool TL_Datetime::operator<(const TL_Datetime& d) {
	return _tv.tv_sec < d._tv.tv_sec || (_tv.tv_sec == d._tv.tv_sec && _tv.tv_usec < d._tv.tv_usec);
}

const bool TL_Datetime::operator==(const TL_Datetime& d) {
	return _tv.tv_sec == d._tv.tv_sec && _tv.tv_usec == d._tv.tv_usec;
}

const bool TL_Datetime::operator<(const time_t& t) const {
	return _tv.tv_sec < t || (_tv.tv_sec == t && _tv.tv_usec > 0);
}

const bool TL_Datetime::operator==(const time_t& t) const {
	return _tv.tv_sec == t && _tv.tv_usec == 0;
}

/*
 TL_Datetime TL_Datetime::operator +(const TL_Datetime& v){
 TL_Datetime d(*this);
 d.
 }
 TL_Datetime& TL_Datetime::operator +=(const TL_Datetime& v){

 }
 */
}
