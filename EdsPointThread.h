/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EdsPointThread.h
 * Author: ever
 *
 * Created on May 1, 2016, 7:39 AM
 */

#ifndef EDSPOINTTHREAD_H
#define EDSPOINTTHREAD_H

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "TL_Thread.h"
#include "TL_Logger.h"
#include "TL_Datetime.h"
//#include "EdsLiveData.h"
#include "LiveClient.h"
#include "ArchClient.h"
#include "KafkaServer.h"
#include "PointData.h"
#include "BitCompress.h"
#include "sqlite3.h"
#include "TL_Mmap.h"
using namespace tidp;
using namespace std;
using namespace eds;
using namespace eds::live;
using namespace eds::arch;
using namespace ever;
#define COMPRESS_TIME_INTVAL 60*60*2 //4小时一个块
#define COMPRESS_BUFFER_LEN 2048
#define EDSPOINTBUFFLEN 4096
#define ONDAY_SECONDS (24 * 60 * 60)
#define BEIJIN_TIME (8 * 60 * 60)

union value_t {
    int i; //值
    float f;
    unsigned int u;
};

struct myPointValue {
    int ts; //时间
    //TL_ThreadRwLock rw_lock;

    value_t value;
    char type;
    char quality; //类型
} __attribute__ ((packed));

struct pointCompress {
    time_t last_ts;
    int last_dealt_ts;

    union value_t {
        int i; //值
        float f;
        unsigned int u;
    } last_value;
    int count;
    BitCompress::BitBuffer bbuf;
    BitCompress::BitZero bz;

    pointCompress() {
        last_ts = 0;
        last_dealt_ts = 0;
        bbuf.pos = 0;
        bz.clz = 0;
        bz.cnz = 32;
        count = 0;
        clear();
    }

    void clear() {
        bbuf.buffer = NULL;
        last_ts = 0;
        last_dealt_ts = 0;
        bbuf.pos = 0;
        bz.clz = 0;
        bz.cnz = 32;
        count = 0;
    }
};

struct compressBlock {
    int block_id;
    int block_start_off;
    int block_size;
    int block_elm_count;
};

struct myPointCache {
    char switch_type; //启停标示  0 1 2
    int pos;
    int block_id;
    unsigned char * mmap_addr;
    int last_record_ts;
    int * bufflen; //当前使用buffer的头长度, |len|count|type|quality|bbuf
    int * count;
    pointCompress comp;
    std::map<int, myPointValue> values; //time value
    //std::map<int, pointCompress> compress;

    myPointCache() {
        mmap_addr = NULL;
        last_record_ts = 0;
        switch_type = 0;
        block_id = -1;
    }
};

extern TL_ThreadLock g_lock;
class EdsPointThread;
extern map<string, pair<EdsPointThread*, int> > _wizid_thread_liveid;
extern map<string, pair<EdsPointThread*, int> > _tgn_thread_liveid;
extern vector<EdsPointThread*> _eds_threads;

class EdsPointThread : public TL_Thread {
public:
    EdsPointThread(LiveClient * live_client, const vector<string> & points, const vector<string> & wizids);
    void setPointFileName(const string& fname);
    void setSwitchData(SwitchData * switchData, const string& switch_topic_name, int parttionNum);
    //
    virtual ~EdsPointThread();
    virtual void run();


    bool checkLiveClient();
    bool initLiveIds();

    void setPointInput();
    void unsetPointInput();
    bool synchronize();
    void getPointsValue(time_t t);
    void record();
    void recordSwitchData();
    void setInterval(int intval);
    void setThreadNo(int tno);
    void checkSwitchData(const string& tgn, time_t t, int val);
    SwitchData::SwitchNode* findSwitchNode(const string&tgn);
    int getThreadNo();
    void setKafkaServer(bool iskafka, KafkaServer * kafka_svr = NULL, const string & topic = "", int partition = -1);
    bool getPointValuesFromCache(int liveid, std::map<int, myPointValue>& values);
    size_t getValidIdSize();
    size_t getInvalidIdSize();

    string status2Json();
    void zeroTsIds2Json(ostream& os);
    void history(ostream& os, const string& tgn, time_t stime, time_t etime);
    void sync2File(const string& filename, const string& point, const myPointValue& pvalue, pointCompress& comp);
    void allockNewBlcok(const string& point, myPointCache& cache);
    void setHistoryRootPath(const string& hrp);
protected:
    EdsPointThread(const EdsPointThread& orig);
    //获取测点的数据并记录,成功返回true,如果返回的时间为0,则表示失败
    bool getLivePointValue(time_t t, int liveid);
private:
    bool _is_record_file;
    bool _is_record_kafka;
    int _save_data_size;
    int _intval_time; //间隔采集时间
    int _elapsed_time;
    int _thread_no;
    struct timeval _t1, _t2;
    map<int, myPointCache> _liveid_pos; //内存保存当前的测点数据,供实时查询
    map<time_t, vector<int> > _timezero_liveids; //获取测点的时间为0 表示无效
    //map<int,int> _pos_liveid;
    //EdsLiveData _eds;
    set<string> _noliveid_points; //无效测点,根本获取不到数据

    const vector<string>& _points;
    const vector<string>& _wizids;
    //map<string,int> _wizid_liveid;
    LiveClient * _live_client;
    TL_ThreadRwLock _rw_lock; //todo 可以做map lock 降低粒度
    vector<string> _kafka_cache;
    vector<string> _kafka_cache2;
    //kafka
    KafkaServer * _kafka_svr;
    SwitchData * _switch_data;
    set<SwitchData::SwitchNode*> _changed_switch_nodes;
    string _switch_topic_name;
    string _topicname;
    string _point_file;
    int _partition_num;
    int _switch_partition_num;
    map<string, SwitchData::SwitchNode*> _switch_data_tmp_cache;
    char * _buffer;
    string history_root_path;
    string history_current_path;
    time_t last_history_time_path;
    sqlite3 * _sqlite3_handler;
    int _datafile_fd;
    int _block_id_max;
    //static sqlite3 * SQLITE3Ptr;
    //static TL_ThreadLock SQLITE3Lock;
};

#endif /* EDSPOINTTHREAD_H */

