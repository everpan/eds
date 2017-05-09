/*
 * File:   TL_Mmap.h
 * Author: everpan
 *
 * Created on 2011年4月25日, 下午2:27
 */

#ifndef TL_MMAP_H
#define	TL_MMAP_H
#include <sys/mman.h>
#include <string>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include "TL_Exp.h"
#include "TL_Common.h"

namespace tidp {

    class TL_Mmap {
    public:
        TL_Mmap(bool autoDestroy = true);
        ~TL_Mmap();
        void mmap(size_t length, int prot, int flags, int fd, off_t offset = 0);
        void mmap(const char *file, size_t length);
        void mmap(const std::string& filename);
        void munmap();
        void static munmap(void *add,int len);
        void msync(bool bSync = false);
        void *getPointer() const;
        size_t getSize() const;
        bool iscreate() const;
        void setAutoDestroy(bool autoDestroy);
    protected:

        bool _bautoDestroy;
        void *_pAddr;
        size_t _iLength;
        bool _bCreate;
    };

}
#endif	/* TL_MMAP_H */

