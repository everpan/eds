/*
 * File:   TL_Common.h
 * Author: everpan
 *
 * Created on 2011年3月15日, 上午11:23
 */

#ifndef TL_COMMON_H
#define	TL_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdarg.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>


#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <stack>
#include <vector>
#include <set>

#include "TL_Exp.h"
using namespace std;
namespace tidp {

class TL_Common {
public:
	TL_Common();
	TL_Common(const TL_Common& orig);
	static void daemon();
	static void daemon2();
	static void ignorePipe();

	static std::string trimRight(const std::string &s);
	static std::string trimLeft(const std::string &s);
	static std::string trim(const std::string &s);

	static void trimRight(const std::string & in,std::string & out);
	static void trimLeft(const std::string & in,std::string& out);
	static void trim(const std::string& in,std::string& out);

	static void split(std::vector<std::string>&vout, const std::string &str, const std::string & splitstr);
	static void split(std::set<std::string>&vset, const std::string &str, const std::string & splitstr);
	static void scanDir(const std::string& dir, std::vector<std::string> &files);
	static void readDir(const std::string& dir, std::vector<std::string> &files);
	static void readDir(DIR * dirp, std::vector<std::string> &files);
	static bool mkdir(const std::string& dir,
			mode_t itype = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	static bool mkdirRecursive(const std::string& dir,
			mode_t itype = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	static bool isFileExistEx(const std::string& file, struct stat* f_stat = NULL, mode_t iFtype = S_IFREG);
	static bool isFileExistEx(const char* file, struct stat* f_stat = NULL, mode_t iFtype = S_IFREG);
	static bool unlink(const std::string& path);
	static bool rename(const std::string& oldpath, const std::string& newpath);
	static bool rename(const char *oldpath, const char *newpath);

	//static size_t strlen(const char *s, size_t def_len = 0);
	static off_t getFileLength(const char *path);
	static off_t getFileLength(int fd);
	static off_t toSize(const std::string& sdesc);
	static bool isBigEndian();

	//sock support
	static void setNonBlock(int fd, bool isBlock = true);
	static int createSocket(int domain = AF_INET, int socktype = SOCK_STREAM);
	static int connect(const string& host, short port);
	static int connectUnix(const string& path);
	static int bind(const string& host,short port);
	static void setSocketReuseAddr(int fd);
	static void setSocketKeepAlive(int fd);
	static void setTcpNoDelay(int fd);
private:

};
}
#endif	/* TL_COMMON_H */

