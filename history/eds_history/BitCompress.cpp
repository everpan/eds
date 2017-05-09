/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BitCompress.cpp
 * Author: ever
 * 
 * Created on 2017年4月6日, 上午9:54
 */

#include "BitCompress.h"
namespace ever {

    BitCompress::BitCompress() {
    }

    BitCompress::BitCompress(const BitCompress& orig) {
    }

    BitCompress::~BitCompress() {
    }

#define SIGNIFICANT_1 7 //min is 7
#define SIGNIFICANT_2 9
#define SIGNIFICANT_3 12 //max is 24

    /**
     * 
     * @param uv
     * @param tail_count must be postive
     * @param bbuf
     */
    void BitCompress::bitCopy(unsigned int uv, unsigned char significant_bit, BitBuffer& bbuf) {
        //取有效位的最左边填充剩余位

        unsigned char reserver = 8 - bbuf.pos;
        if (significant_bit < reserver) {
            *bbuf.buffer |= (unsigned char) (uv << (reserver - significant_bit));
            bbuf.pos += significant_bit;
            return;
        } else if (significant_bit == reserver) {
            *bbuf.buffer |= (unsigned char) uv;
            ++bbuf.buffer;
            *bbuf.buffer = 0;
            bbuf.pos = 0;
            return;
        }

        *bbuf.buffer |= (unsigned char) (uv >> (significant_bit - reserver));
        ++bbuf.buffer;
        *bbuf.buffer = 0;
        bbuf.pos = 0;
        significant_bit -= reserver;

        uv <<= (32 - significant_bit);

        unsigned char * p = (unsigned char *) &uv;
        unsigned char pos = 3;
        //小端对齐模式拷贝
        while (significant_bit > 0) {
            *bbuf.buffer = p[pos];
            if (significant_bit < 8) {
                bbuf.pos = significant_bit;
                break;
            } else if (significant_bit == 8) {
                ++bbuf.buffer;
                *bbuf.buffer = 0;
                bbuf.pos = 0;
                break;
            } else {
                ++bbuf.buffer;
                --pos;
                bbuf.pos = 0;
                significant_bit -= 8;
            }
        };
    }

    /**
     * 
     * @param v
     * @param bbuf buff不判断是否越界,调用时候确保有5个字节以上的空间
     */
    void BitCompress::compressInt(int v, BitBuffer& bbuf) {
        //cout << __FUNCTION__ << " ___________________ " << v << "," << (int) bbuf.pos << endl;
        if (v == 0) {//store zero
            ++bbuf.pos;
            if (bbuf.pos == 8) {
                ++bbuf.buffer;
                *bbuf.buffer = 0;
                bbuf.pos = 0;
            }
        } else {
            bool sign = false; //positive
            unsigned uv = 0;
            if (v < 0) {
                sign = true; //negative
                uv = abs(v);
            } else {
                uv = v;
            }
            //unsigned char head_sign_bit = 0;
            unsigned char significant_bit = 0;
            if (uv < (1 << (SIGNIFICANT_1 - 1)) - 1) {
                if (sign) {//101
                    uv |= (0x5 << (SIGNIFICANT_1 - 1));
                } else {
                    uv |= (0x4 << (SIGNIFICANT_1 - 1));
                }
                significant_bit = SIGNIFICANT_1 + 2;
            } else if (uv < (1 << (SIGNIFICANT_2 - 1)) - 1) {
                if (sign) {//1101
                    uv |= (0xd << (SIGNIFICANT_2 - 1));
                } else {
                    uv |= (0xc << (SIGNIFICANT_2 - 1));
                }
                significant_bit = SIGNIFICANT_2 + 3;
            } else if (uv < (1 << (SIGNIFICANT_3 - 1)) - 1) {
                if (sign) {//11101
                    uv |= (0x1d << (SIGNIFICANT_3 - 1));
                } else {
                    uv |= (0x1c << (SIGNIFICANT_3 - 1));
                }
                significant_bit = SIGNIFICANT_3 + 4;
            } else {
                if (sign) {
                    bitCopy(0x1f, 5, bbuf);
                } else {
                    bitCopy(0x1e, 5, bbuf);
                }
                bitCopy(uv, 31, bbuf);
                return;
            }
            bitCopy(uv, significant_bit, bbuf);
        }
    }

    /**
     * 
     * @param v 与前一个值xor后的值
     * @param bbuf
     * @param bz 前一个值的前导0的数量和有效比特位的数量
     */
    void BitCompress::compressXORInt(unsigned int v, BitBuffer& bbuf, BitZero& bz) {
        if (v == 0) {
            ++bbuf.pos;
            if (bbuf.pos == 8) {
                ++bbuf.buffer;
                * bbuf.buffer = 0;
                bbuf.pos = 0;
            }
            return;
        }
        BitZero bzself;
        bzself.clz = __builtin_clz(v);
        int ctz = __builtin_ctz(v);
        bzself.cnz = 32 - bzself.clz - ctz;

        if (bz.clz == bzself.clz && bz.cnz == bzself.cnz) {
            //10
            bitCopy(2, 2, bbuf);
            bitCopy((v >> ctz), bzself.cnz, bbuf);
        } else {
            //11
            int vv = (bzself.clz | 0x60) ;
            vv <<= 6;
            vv |= bzself.cnz;
            bitCopy(vv, 13, bbuf);
            vv = (v >> ctz);
            bitCopy(vv, bzself.cnz, bbuf);
        }
    }

    int BitCompress::uncompressInt(BitBuffer& bbuf) {
        unsigned int v;
        //小端读取模式
        unsigned char * p = (unsigned char *) &v;
        p[0] = bbuf.buffer[3];
        p[1] = bbuf.buffer[2];
        p[2] = bbuf.buffer[1];
        p[3] = bbuf.buffer[0];

        v <<= bbuf.pos;

        unsigned char sign = (p[3] >> 3);

        if (sign < 0x10) { //zero
            ++bbuf.pos;
            if (bbuf.pos == 8) {
                ++bbuf.buffer;
                bbuf.pos = 0;
            }
            return 0;
        } else if (sign < 0x14) {//positive 10,0
            ++bbuf.buffer;
            bbuf.pos += (SIGNIFICANT_1 + 2 - 8);
            if (bbuf.pos == 8) {
                ++bbuf.buffer;
                bbuf.pos = 0;
            }
            return (v << 3) >> (33 - SIGNIFICANT_1);
        } else if (sign < 0x18) {//negative 10,1
            ++bbuf.buffer;
            bbuf.pos += (SIGNIFICANT_1 + 2 - 8);
            if (bbuf.pos == 8) {
                ++bbuf.buffer;
                bbuf.pos = 0;
            }
            return -1 * ((v << 3) >> (33 - SIGNIFICANT_1));
        } else if (sign < 0x1a) {//positive 110,0
            bbuf.pos += SIGNIFICANT_2 + 3;
            while (bbuf.pos > 7) {
                ++bbuf.buffer;
                bbuf.pos -= 8;
            }
            return (v << 4) >> (33 - SIGNIFICANT_2);
        } else if (sign < 0x1c) {//negative 110,1
            bbuf.pos += SIGNIFICANT_2 + 3;
            while (bbuf.pos > 7) {
                ++bbuf.buffer;
                bbuf.pos -= 8;
            }
            return -1 * ((v << 4) >> (33 - SIGNIFICANT_2));
        } else if (sign == 0x1c) { //positive 1110,0
            bbuf.pos += SIGNIFICANT_3 + 4;
            while (bbuf.pos > 7) {
                ++bbuf.buffer;
                bbuf.pos -= 8;
            }

            return (v << 5) >> (33 - SIGNIFICANT_3);
        } else if (sign == 0x1d) { //1110,1
            bbuf.pos += SIGNIFICANT_3 + 4;
            while (bbuf.pos > 7) {
                ++bbuf.buffer;
                bbuf.pos -= 8;
            }
            return -1 * ((v << 5) >> (33 - SIGNIFICANT_3));
        } else if (sign == 0x1e) {
            unsigned long long lv;
            unsigned char * lp = (unsigned char *) &lv;

            lp[3] = bbuf.buffer[4];
            lp[4] = bbuf.buffer[3];
            lp[5] = bbuf.buffer[2];
            lp[6] = bbuf.buffer[1];
            lp[7] = bbuf.buffer[0];
            //cout << bitset<64>(lv);
            lv <<= (bbuf.pos + 5);
            lv >>= 1;
            p[3] = lp[7];
            p[2] = lp[6];
            p[1] = lp[5];
            p[0] = lp[4];
            bbuf.pos += 4 + 32;
            while (bbuf.pos > 7) {
                ++bbuf.buffer;
                bbuf.pos -= 8;
            }
            return v;
        } else { //1111x 1110,1111
            unsigned long long lv;
            unsigned char * lp = (unsigned char *) &lv;

            lp[3] = bbuf.buffer[4];
            lp[4] = bbuf.buffer[3];
            lp[5] = bbuf.buffer[2];
            lp[6] = bbuf.buffer[1];
            lp[7] = bbuf.buffer[0];
            //cout << bitset<64>(lv);
            lv <<= (bbuf.pos + 5);
            lv >>= 1;
            p[3] = lp[7];
            p[2] = lp[6];
            p[1] = lp[5];
            p[0] = lp[4];
            bbuf.pos += 4 + 32;
            while (bbuf.pos > 7) {
                ++bbuf.buffer;
                bbuf.pos -= 8;
            }
            return -v;
        }


        //bbuf.buffer;
        return -9999;
    }

    unsigned int BitCompress::uncompressXORInt(BitBuffer& bbuf, BitZero& bz) {
        unsigned long long lv;
        unsigned char * lp = (unsigned char *) & lv;
        lp[0] = bbuf.buffer[7];
        lp[1] = bbuf.buffer[6];
        lp[2] = bbuf.buffer[5];
        lp[3] = bbuf.buffer[4];
        lp[4] = bbuf.buffer[3];
        lp[5] = bbuf.buffer[2];
        lp[6] = bbuf.buffer[1];
        lp[7] = bbuf.buffer[0];
        lv <<= bbuf.pos;
        int flag = (lp[7] >> 6);
        if (flag < 2) { //00,01
            ++bbuf.pos;
            if (bbuf.pos == 8) {
                ++bbuf.buffer;
                bbuf.pos = 0;
            }
            return 0;
        } else if (flag == 2) { //10
            lv <<= 2;
            lv >>= (64 - bz.cnz);
            lv <<= (32 - bz.clz - bz.cnz);
            bbuf.pos += (2 + bz.cnz);
            while (bbuf.pos > 7) {
                ++bbuf.buffer;
                bbuf.pos -= 8;
            }
            return lv;
        } else {
            lv <<= 2;
            bz.clz = (lp[7] >> 3);
            lv <<= 5;
            bz.cnz = (lp[7] >> 2);
            lv <<= 6;

            lv >>= (64 - bz.cnz);
            lv <<= (32 - bz.clz - bz.cnz);
            bbuf.pos += (13 + bz.cnz);
            while (bbuf.pos > 7) {
                ++bbuf.buffer;
                bbuf.pos -= 8;
            }
            return lv;
        }
        return -1;
    }
}
