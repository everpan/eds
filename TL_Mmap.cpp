/*
 * File:   TL_Mmap.cpp
 * Author: everpan
 *
 * Created on 2011年4月25日, 下午2:27
 */

#include "TL_Mmap.h"
namespace tidp {

    TL_Mmap::TL_Mmap(bool autoDestroy)
    : _bautoDestroy(autoDestroy)
    , _pAddr(NULL)
    , _iLength(0)
    , _bCreate(false) {
    }

    TL_Mmap::~TL_Mmap() {
        if (_bautoDestroy) {
            munmap();
        }
    }

    void TL_Mmap::mmap(size_t length, int prot, int flags, int fd, off_t offset) {
        if (_pAddr) {
            munmap();
        }
        _pAddr = ::mmap(NULL, length, prot, flags, fd, offset);
        
        if (_pAddr == (void*) - 1) {
            _pAddr = NULL;
            throw TL_Exp("[TL_Mmap::mmap] mmap error", errno);
        }
        _iLength = length;
        _bCreate = false;
    }

    void TL_Mmap::mmap(const char *file, size_t length) {
        assert(length > 0);
        if (_pAddr) {
            munmap();
        }
        //注意_bCreate的赋值位置:保证多线程用一个对象的时候也不会有问题
        int fd = open(file, O_CREAT | O_EXCL | O_RDWR, 0666);
        if (fd == -1) {
            if (errno != EEXIST) {
                throw TL_Exp("[TL_Mmap::mmap] fopen file '" + std::string(file) + "' error", errno);
            } else {
                fd = open(file, O_CREAT | O_RDWR, 0666);
                if (fd == -1) {
                    throw TL_Exp("[TL_Mmap::mmap] fopen file '" + std::string(file) + "' error", errno);
                }
                _bCreate = false;
            }
        } else {
            _bCreate = true;
        }

        lseek(fd, length - 1, SEEK_SET);
        write(fd, "\0", 1);

        _pAddr = ::mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (_pAddr == (void*) - 1) {
            _pAddr = NULL;
            close(fd);
            throw TL_Exp("[TL_Mmap::mmap] mmap file '" + std::string(file) + "' error", errno);
        }
        _iLength = length;
        if (fd != -1) {
            close(fd);
        }
    }

    void TL_Mmap::mmap(const std::string& filename) {
        off_t flen = TL_Common::getFileLength(filename.c_str());
        if (flen <= 0) {
            throw TL_Exp("[TL_Mmap::mmap] mmap file '" + filename + "' error.");
        }
        this->mmap(filename.c_str(), flen);
    }

    void TL_Mmap::munmap() {
        if (_pAddr == NULL) {
            return;
        }

        int ret = ::munmap(_pAddr, _iLength);
        if (ret == -1) {
            throw TL_Exp("[TL_Mmap::munmap] munmap error", errno);
        }

        _pAddr = NULL;
        _iLength = 0;
        _bCreate = false;
    }

    void TL_Mmap::munmap(void *add, int len) {
        if (add == NULL) {
            return;
        }

        int ret = ::munmap(add, len);
        if (ret == -1) {
            throw TL_Exp("[TL_Mmap::munmap] munmap error", errno);
        }
    }

    void TL_Mmap::msync(bool bSync) {
        int ret = 0;
        if (bSync) {
            ret = ::msync(_pAddr, _iLength, MS_SYNC | MS_INVALIDATE);
        } else {
            ret = ::msync(_pAddr, _iLength, MS_ASYNC | MS_INVALIDATE);
        }
        if (ret != 0) {
            throw TL_Exp("[TL_Mmap::msync] msync error", errno);
        }
    }

    void * TL_Mmap::getPointer() const {
        return _pAddr;
    }

    size_t TL_Mmap::getSize() const {
        return _iLength;
    }

    bool TL_Mmap::iscreate() const {
        return _bCreate;
    }

    void TL_Mmap::setAutoDestroy(bool autoDestroy) {
        _bautoDestroy = autoDestroy;
    }

}
