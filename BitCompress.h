/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BitCompress.h
 * Author: ever
 *
 * Created on 2017年4月6日, 上午9:54
 */

#ifndef BITCOMPRESS_H
#define BITCOMPRESS_H

#include <stdlib.h>
#include <iostream>
using namespace std;
namespace ever {

    class BitCompress {
    public:
        BitCompress();
        BitCompress(const BitCompress& orig);
        virtual ~BitCompress();

        typedef struct stru_bit_buffer {
            unsigned char * buffer;
            unsigned char pos;
        } BitBuffer;

        typedef struct stru_bit_zero {
            unsigned char clz;
            unsigned char cnz; //有效位
        } BitZero;

        static void compressInt(int v, BitBuffer& bbuf);
        static void compressXORInt(unsigned int v, BitBuffer& bbuf, BitZero& bz);
        
        static void bitCopy(unsigned int v, unsigned char tail_count/*尾部存储位数*/, BitBuffer& bbuf);
        
        static int uncompressInt(BitBuffer& bbuf);
        static unsigned int uncompressXORInt(BitBuffer& bbuf, BitZero& bz);
    private:

    };
}//end namespace ever
#endif /* BITCOMPRESS_H */

