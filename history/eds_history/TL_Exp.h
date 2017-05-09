/*
 * File:   TL_Exp.h
 * Author: everpan
 *
 * Created on 2011年3月15日, 上午11:48
 */

#ifndef TL_EXP_H
#define	TL_EXP_H

#include <stdexcept>
#include <string>
#include <string.h>
#include <sstream>
namespace tidp {

class TL_Exp: public std::exception {
public:
	explicit TL_Exp(const std::string &buffer);
	TL_Exp(const std::string &buffer, int err);
	TL_Exp(const std::string &fuction, const std::string& msg, int lineno);
	TL_Exp(const std::string& file, int lineno, const std::string& msg, int errcode) throw ();
	TL_Exp(const char * file, int lineno, const char * func, const std::string& msg, int errcode) throw ();
	virtual ~TL_Exp() throw ();
	virtual const char* what() const throw ();

	int getErrCode() {
		return _code;
	}
	//void getBacktrace();
private:
	//std::string _className;
	std::string _buffer;
	int _line_no;
	int _code;
};
#define EXCEPTION(msg,ercode) throw tidp::TL_Exp(__FILE__,__LINE__,msg,ercode)
#define TL_Exception(msg,errcode) tidp::TL_Exp(__FILE__,__LINE__,__FUNCTION__,msg,errcode)
}
#endif	/* TL_EXP_H */

