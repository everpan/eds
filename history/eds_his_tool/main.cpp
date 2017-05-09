/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: ever
 *
 * Created on 2017年4月12日, 下午3:01
 */

#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../../BitCompress.h"
#include "../../sqlite3.h"
#include "../../TL_Mmap.h"



#define EDSPOINTBUFFLEN 4096
using namespace std;
using namespace ever;
using namespace tidp;

/*
 * 
 */

struct compressBlock {
    int block_id;
    int block_start_off;
    int block_size;
    int block_elm_count;
};

union value_t {
    int i; //值
    float f;
    unsigned int u;
};

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "tool tid tgn blockid offset\n"
                "  tid  thread id\n"
                "  tgn  target name\n"
                "  blockid blockid\n"
                "  offset start offset\n" << endl;
        exit(-1);
    }
    int tid = -1;
    if (argc >= 2) {
        tid = atoi(argv[1]);
    }
    string tgn;
    if (argc >= 3) {
        tgn = argv[2];
    }

    int blockid = -1;
    int start_off = 0;

    if (argc >= 4) {
        blockid = atoi(argv[3]);
    }

    if (argc >= 5) {
        start_off = atoi(argv[4]);
    }

    sqlite3 * db;
    char tmp[256];
    sprintf(tmp, "tgn_%d.db", tid);
    int ret = sqlite3_open(tmp, &db);
    if (ret != 0) {
        cerr << "sqlite open " << tgn << ".db failed." << endl;
        return -1;
    }

    string sql = "select * from t_tgn where tgn = \"";
    sql += tgn + "\"";

    sqlite3_stmt * stmt;
    sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, 0);
    int row = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ++row;
        cout << "(" << sqlite3_column_int(stmt, 1) << "," << sqlite3_column_int(stmt, 2) << ")" << endl;
    }
    if (row == 0) {
        cerr << "there is no data for " << tgn << endl;
        return -1;
    }

    if (blockid >= 0) {
        sprintf(tmp, "tgn_%d.data", tid);
        int fd = open(tmp, O_RDONLY);
        if (fd < 0) {
            cerr << "open " << tmp << " failed." << endl;
            return -1;
        }
        TL_Mmap mp;
        mp.mmap(EDSPOINTBUFFLEN, PROT_READ, MAP_SHARED | MAP_FILE, fd, blockid * EDSPOINTBUFFLEN);
        close(fd);
        int * len = ((int *) ((char *)mp.getPointer() + start_off));
        int * count = len + 1;
        char type = *((char *) (count + 1));
        char quality = *((char *) (count + 1) + 1);

        cout << "tgn:" << tgn << ",type:" << type << ",quality:" << quality
                << ",block:" << blockid
                << ",start offset:" << start_off
                << ",bufflen:" << *len
                << ",elm_size:" << *count << endl;


        BitCompress::BitBuffer bbuf;
        BitCompress::BitZero bz;
        bbuf.buffer = (unsigned char *) ((unsigned char *) mp.getPointer() + 10);
        bbuf.pos = 0;
        bz.clz = 0;
        bz.cnz = 32;

        int k = 0;
        time_t t = 0;

        time_t last_tm = 0;
        value_t last_v;

        unsigned int bxor = 0;
        int last_dt = 0;
        if (*count > 0) { //初值
            last_tm = BitCompress::uncompressInt(bbuf);
            last_v.u = BitCompress::uncompressXORInt(bbuf, bz);
        }

        if (type != 'B' && type != 'P') {
            //float
            cout << "(" << last_tm << "," << last_v.f << ")" << endl;
        } else {
            cout << "(" << last_tm << "," << last_v.i << ")" << endl;
        }

        unsigned char * buf_start = (unsigned char *) mp.getPointer() + 10;

        int bit1, bit2, bit3;
        for (int i = 1; i < *count; ++i) {
            bit1 = (bbuf.buffer - buf_start)*8L + bbuf.pos;
            cout << i << "\t" << bit1 << " (" << bbuf.buffer - buf_start << "," << (int) bbuf.pos << ") ";
            k = BitCompress::uncompressInt(bbuf);
            last_tm += k + last_dt;
            last_dt = k;
            bit2 = (bbuf.buffer - buf_start)*8L + bbuf.pos;
            cout << (bit2 - bit1) << "," << bit2 << " (" << bbuf.buffer - buf_start << "," << (int) bbuf.pos << ") ";
            bxor = BitCompress::uncompressXORInt(bbuf, bz);
            bit3 = (bbuf.buffer - buf_start)*8L + bbuf.pos;
            cout << (bit3 - bit2) << "," << bit3 << " (" << bbuf.buffer - buf_start << "," << (int) bbuf.pos << ")," << bxor;
            last_v.u ^= bxor;

            if (type != 'B' && type != 'P') {
                //float
                cout << "(" << last_tm << "," << last_v.f << ")" << endl;
            } else {
                cout << "(" << last_tm << "," << last_v.i << ")" << endl;
            }
            //cout << 
        }
    }



    return 0;
    /*    FILE * fp = fopen(argv[1], "r");
        if (!fp) {
            cerr << "open " << argv[1] << " failed. " << errno << ":" << strerror(errno) << endl;
            exit(-1);
        }
        int block_id = 0;
        if (argc > 2) {
            block_id = atoi(argv[2]);
        }
        char * buffer = (char *) malloc(2048);
        fread(buffer, 1, 1024, fp);
        compressBlock * block = (compressBlock*) (buffer + 64);
        compressBlock tag_block;
        tag_block.block_id = 0;
        char type = buffer[62];
        char quality = buffer[63];

        cout << "tgn:" << buffer << ",type:" << type << ",quality:" << quality << endl;
        cout << "############### block info #####################" << endl;
        while (block->block_id != 0) {
            cout << "id=" << block->block_id
                    << ",size=" << block->block_size
                    << ",off=" << block->block_start_off
                    << ",elm_count=" << block->block_elm_count << endl;
            if (block->block_id == block_id) {
                tag_block = *block;
            }
            ++block;
        };
        cout << "############### block info #####################" << endl;

        if (tag_block.block_id > 0) {
            int fd = fileno(fp);
            //char type = buffer

            pread(fd, buffer, tag_block.block_size, tag_block.block_start_off);
            unsigned int last_v = 0;
            //last_v.u = 0;
            value_t v;
            BitCompress::BitBuffer bbuf;
            BitCompress::BitZero bz;
            bbuf.buffer = (unsigned char *) buffer;
            bbuf.pos = 0;
            bz.clz = 0;
            bz.cnz = 32;
            time_t last_tm = 0;
            int delat = 0;
            int k = 0;
            time_t t = 0;



            for (int i = 0; i < tag_block.block_elm_count; ++i) {
                k = BitCompress::uncompressInt(bbuf);
                t = k + last_tm + delat;
                last_tm = t;
                delat = k;

                v.u = BitCompress::uncompressXORInt(bbuf, bz);
                if (i == 0) {
                    last_v = v.u;
                } else {
                    last_v |= v.u;
                }
                if (type != 'B' && type != 'P') {
                    //float
                    cout << "(" << t << "," << v.f << ")" << endl;
                } else {
                    cout << "(" << t << "," << v.i << ")" << endl;
                }
            }
        }
        fclose(fp);
        free(buffer);
     */
    return 0;
}

