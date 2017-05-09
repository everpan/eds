/*
 * File:   TL_Common.cpp
 * Author: everpan
 *
 * Created on 2011年3月15日, 上午11:23
 */

#include "TL_Common.h"
namespace tidp {

void TL_Common::daemon() {
	pid_t pid;

	if ((pid = fork()) != 0) {
		exit(0);
	}
	setsid();
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	//signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	ignorePipe();

	if ((pid = fork()) != 0) { //父进程结束,变成daemon
		exit(0);
	}
	umask(022);
}

void TL_Common::daemon2() {
	pid_t pid;

	if ((pid = fork()) != 0) {
		exit(0);
	}
	setsid();
	signal(SIGINT, SIG_IGN);
	//signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	//signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	//signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	ignorePipe();

	if ((pid = fork()) != 0) { //父进程结束,变成daemon
		exit(0);
	}
	umask(022);
}

void TL_Common::ignorePipe() {
	struct sigaction sig;

	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE, &sig, NULL);
}

std::string TL_Common::trimRight(const std::string &s) {
	if (s.size() == 0)
		return s;
	int i = s.size() - 1;
	while (i > -1) {
		const char & c = s[i];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			--i;
		else
			break;
	}
	return s.substr(0, i + 1);
}

std::string TL_Common::trimLeft(const std::string &s) {
	if (s.size() == 0)
		return s;
	size_t i = 0;
	size_t n = s.size();

	do {
		const char& c = s[i];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			++i;
		else
			break;
	} while (i < n);

	return s.substr(i);
}

std::string TL_Common::trim(const std::string &s) {
	return trimRight(trimLeft(s));
}

void TL_Common::trimRight(const std::string & in, std::string & out) {
	if (in.size() == 0) {
		out.clear();
		return;
	}
	int i = in.size() - 1;
	while (i > -1) {
		const char & c = in[i];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			--i;
		else
			break;
	}
	out = in.substr(0, i + 1);
}

void TL_Common::trimLeft(const std::string & in, std::string& out) {
	if (in.size() == 0) {
		out.clear();
		return;
	}
	size_t i = 0;
	size_t n = in.size();
	do {
		const char& c = in[i];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			++i;
		else
			break;
	} while (i < n);
	out = in.substr(i);
}
void TL_Common::trim(const std::string& in, std::string& out) {
	if (in.size() == 0) {
		out.clear();
		return;
	}
	int i = 0;
	int j = in.size();
	do {
		const char& c = in[i];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			++i;
		else
			break;
	} while (i < j);

	--j;
	if (j == i) {
		out.clear();
		return;
	}
	do {
		const char& c = in[j];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			--j;
		else
			break;
	} while (j > i);
	if (j <= i) {
		out.clear();
		return;
	} else {
		out.assign(&in[i], j - i + 1);
	}
}

void TL_Common::split(std::vector<std::string> &vout, const std::string &str, const std::string & splitstr) {
	vout.clear();
	if (str.size() == 0)
		return;
	int split_size = splitstr.size();
	if (split_size == 0) {
		vout.push_back(str);
		return;
	}
	int split_i = 0;
	const char *s = str.c_str();
	const char *e = s + str.size();
	const char *p = s;

	while (s < e) {

		for (split_i = 0; split_i < split_size; ++split_i) {
			if (*s == splitstr[split_i]) {
				vout.push_back(std::string(p, s - p));
				p = s + 1;
			}
		}
		++s;
	}
	if (s > p) {
		vout.push_back(std::string(p, s - p));
	}
}

void TL_Common::split(std::set<std::string>& vset, const std::string &str, const std::string & splitstr) {
	vset.clear();
	if (str.size() == 0)
		return;
	int split_size = splitstr.size();
	if (split_size == 0) {
		vset.insert(str);
		return;
	}
	int split_i = 0;
	const char *s = str.c_str();
	const char *e = s + str.size();
	const char *p = s;

	while (s < e) {

		for (split_i = 0; split_i < split_size; ++split_i) {
			if (*s == splitstr[split_i]) {
				vset.insert(std::string(p, s - p));
				p = s + 1;
			}
		}
		++s;
	}
	if (s > p) {
		vset.insert(std::string(p, s - p));
	}
}

void TL_Common::scanDir(const std::string& dir, std::vector<std::string> &files) {
	files.clear();
	struct dirent **namelist;
	int n;

	n = scandir(dir.c_str(), &namelist, 0, alphasort);
	if (n < 0)
		throw TL_Exp(std::string("scanDir error:") + dir);
	else {
		while (n--) {
			files.push_back(namelist[n]->d_name);
			free(namelist[n]);
		}
		free(namelist);
	}
}

void TL_Common::readDir(const std::string& dir, std::vector<std::string>& files) {
	DIR* dirp = opendir(dir.c_str());
	if (dirp == NULL) {
		throw TL_Exp("TL_Common::readDir:can not open dir=" + dir, errno);
		return;
	}
	readDir(dirp, files);
	closedir(dirp);
}

void TL_Common::readDir(DIR * dirp, std::vector<std::string> &files) {
	files.clear();
	if (dirp == NULL) {
		throw TL_Exp("TL_Common::readDir DIR * dirp is NULL.");
		return;
	}
	struct dirent *direntp = NULL;
	while ((direntp = readdir(dirp)) != NULL) {
		files.push_back(direntp->d_name);
	}
}

bool TL_Common::isFileExistEx(const std::string& file, struct stat* f_stat, mode_t iFtype) {
	return TL_Common::isFileExistEx(file.c_str(), f_stat, iFtype);
}

bool TL_Common::isFileExistEx(const char* file, struct stat* f_stat, mode_t iFtype) {
	if (f_stat != NULL) {
		if (stat(file, f_stat) == -1) {
			return false;
		}
		if (!(f_stat->st_mode & iFtype)) {
			return false;
		}
	} else {
		struct stat f_stat;
		if (stat(file, &f_stat) == -1) {
			return false;
		}
		if (!(f_stat.st_mode & iFtype)) {
			return false;
		}
	}
	return true;
}

bool TL_Common::mkdir(const std::string& dir, mode_t itype) {
	if (isFileExistEx(dir, NULL, S_IFDIR))
		return true;
	int iRetCode = ::mkdir(dir.c_str(), itype);
	if (iRetCode < 0 && errno == EEXIST) {
		return false; //isFileExistEx(dir, NULL, S_IFDIR);
	}
	return iRetCode == 0;
}

bool TL_Common::mkdirRecursive(const std::string& dir, mode_t itype) {
	const char *p1, *p2;
	p1 = p2 = dir.c_str();
	while (*p2 != '\0') {
		if (*p2 == '/' && p2 != p1) {
			if (!mkdir(string(p1, p2 - p1), itype)) {
				//cout << "mkdir:" << string(p1,p2-p1) << " false." << endl;
				return false;
			}

		}
		++p2;
	}
	return true;
}
bool TL_Common::unlink(const std::string& path) {
	int ret = ::unlink(path.c_str());
	if (ret == 0) {
		return true;
	} else {
		std::string s = "TL_Common::unlink " + path + " fail.";
		throw TL_Exp(s, errno);
	}
}

bool TL_Common::rename(const std::string& oldpath, const std::string& newpath) {
	return TL_Common::rename(oldpath.c_str(), newpath.c_str());
}

bool TL_Common::rename(const char *oldpath, const char *newpath) {
	int ret = ::rename(oldpath, newpath);
	if (ret == 0) {
		return true;
	} else {
		std::string s = "TL_Common::rename ";
		s += oldpath;
		s += " to ";
		s += newpath;
		s += " fail.";
		throw TL_Exp(s, errno);
	}
	return false;
}
/*
 size_t TL_Common::strlen(const char* s, size_t def_len) {
 if (s == NULL) return 0;
 //if(strlen(s) > def_len) return def_len;
 size_t len = ::strlen(s);
 if (def_len > 0 && len > def_len) return def_len;
 else return len;
 }
 */
off_t TL_Common::getFileLength(const char* path) {
	struct stat st;
	int ret = stat(path, &st);
	if (ret != 0) {
		throw TL_Exp(std::string("TL_Common::getFileLength:stat fail.[") + path + "]", errno);
		return -1;
	}
	return st.st_size;
}

off_t TL_Common::getFileLength(int fd) {
	struct stat st;
	int ret = fstat(fd, &st);
	if (ret != 0) {
		throw TL_Exp("TL_Common::getFileLength:stat fail.", errno);
		return -1;
	}
	return st.st_size;
}

off_t TL_Common::toSize(const std::string& sdesc) {
	if (sdesc.size() == 0)
		return 0;
	char c = sdesc[sdesc.size() - 1];
	off_t n = atol(sdesc.c_str());
	switch (c) {
		case 'k':
		case 'K':
			return n * 1024;
		case 'm':
		case 'M':
			return n * 1024 * 1024;
		case 'g':
		case 'G':
			return n * 1024 * 1024 * 1024;
		case 't':
		case 'T':
			return n * 1024 * 1024 * 1024 * 1024LL;
		case 'p':
		case 'P':
			return (unsigned long long) n * 1024 * 1024 * 1024 * 1024 * 1024LL;
		default:
			return n;
			break;
	}
	return 0;
}

void TL_Common::setNonBlock(int fd, bool isBlock) {
	int val = 0;

	if ((val = fcntl(fd, F_GETFL, 0)) == -1) {
		throw TL_Exp("[TL_Common::setNonBlock] fcntl [F_GETFL] error.", errno);
	}

	if (isBlock) {
		val |= O_NONBLOCK;
	} else {
		val &= ~O_NONBLOCK;
	}

	if (fcntl(fd, F_SETFL, val) == -1) {
		throw TL_Exp("[TL_Common::setNonBlock] fcntl [F_SETFL] error.", errno);
	}
}
int TL_Common::createSocket(int domain, int socktype) {
	int sock = socket(domain, socktype, 0);
	if (sock == -1) {
		throw TL_Exp("[TL_Common::createSocket] socket error.", errno);
	}
	return sock;
}
int TL_Common::connect(const string& host, short port) {
	int sock = createSocket();
	struct sockaddr stServerAddr;
	bzero(&stServerAddr, sizeof(stServerAddr));

	struct sockaddr_in *p = (struct sockaddr_in *) &stServerAddr;

	p->sin_family = AF_INET;
	int ret = inet_pton(AF_INET, host.c_str(), &p->sin_addr);
	if (ret < 0) {
		throw TL_Exception("[TL_Common::connect] inet_pton error.", errno);
	}
	p->sin_port = htons(port);

	if (::connect(sock, (struct sockaddr *) &stServerAddr, sizeof(struct sockaddr)) != 0) {
		ostringstream os;
		os << "[TL_Common::connect] connect to " << host << ":" << port << " error";
		throw TL_Exception(os.str(), errno);
	}
	return sock;
}
int TL_Common::connectUnix(const string& path) {
	struct sockaddr_un sa;
	int sock = createSocket(AF_LOCAL, SOCK_STREAM);
	sa.sun_family = AF_LOCAL;
	strncpy(sa.sun_path, path.c_str(), sizeof(sa.sun_path) - 1);
	if (::connect(sock, (struct sockaddr*) &sa, sizeof(sa)) == -1) {
		throw TL_Exception("connect error.", errno);
	}
	return sock;
}
int TL_Common::bind(const string& host, short port) {
	int sock = createSocket();
	struct sockaddr stServerAddr;
	bzero(&stServerAddr, sizeof(stServerAddr));

	struct sockaddr_in *p = (struct sockaddr_in *) &stServerAddr;

	p->sin_family = AF_INET;
	int ret = inet_pton(AF_INET, host.c_str(), &p->sin_addr);
	if (ret < 0) {
		throw TL_Exception("[TL_Common::connect] inet_pton error.", errno);
	}
	p->sin_port = htons(port);
	setSocketReuseAddr(sock);
	ret = ::bind(sock, &stServerAddr, sizeof(struct sockaddr));
	if (ret < 0) {
		throw TL_Exception("[TL_Common::connect] bind error.", errno);
	}
	return sock;
}

void TL_Common::setSocketReuseAddr(int fd) {
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
		throw TL_Exception("SO_REUSEADDR failed.", errno);
	}
}
void TL_Common::setSocketKeepAlive(int fd) {
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) == -1) {
		throw TL_Exception("SO_KEEPALIVE failed.", errno);
	}
}
void TL_Common::setTcpNoDelay(int fd) {
	int yes = 1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
		throw TL_Exception("TCP_NODELAY failed.", errno);
	}
}
}
