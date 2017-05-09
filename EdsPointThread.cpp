/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EdsPointThread.cpp
 * Author: ever
 * 
 * Created on May 1, 2016, 7:39 AM
 */

#include "EdsPointThread.h"
TL_ThreadLock g_lock;
map<string, pair<EdsPointThread*, int> > _wizid_thread_liveid;
map<string, pair<EdsPointThread*, int> > _tgn_thread_liveid;
vector<EdsPointThread*> _eds_threads;

//sqlite3 * SQLITE3Ptr = NULL;
//TL_ThreadLock SQLITE3Lock;

EdsPointThread::EdsPointThread(LiveClient * live_client, const vector<string> & points, const vector<string> & wizids) :
_live_client(live_client),
_points(points), _wizids(wizids) {
    _is_record_file = true;
    _is_record_kafka = false;
    _save_data_size = 1800; //保留最近x个数据
    _intval_time = 1; //默认1s采集一次
    _kafka_cache.resize(points.size());
    if (!checkLiveClient()) {
        LOG() << error << "EdsPointThread: init is null" << endl;
        return;
    }
    _switch_data = NULL;
    _buffer = (char *) malloc(EDSPOINTBUFFLEN);
    history_root_path = "/tmp";
    last_history_time_path = 0;
    _sqlite3_handler = NULL;
    _datafile_fd = -1;
    _block_id_max = -1;
}

void EdsPointThread::setPointFileName(const string& fname) {
    _point_file = fname;
}

void EdsPointThread::setSwitchData(SwitchData * switchData, const string& switch_topic_name, int parttionNum) {
    _switch_data = switchData;
    _switch_topic_name = switch_topic_name;
    _switch_partition_num = parttionNum;
}

EdsPointThread::~EdsPointThread() {
    if (_buffer) {
        free(_buffer);
    }
}

void EdsPointThread::run() {
    unsigned long long usleep_time;
    initLiveIds();
    _elapsed_time = 0;
    try {
        do {
            gettimeofday(&_t1, NULL);
            setPointInput();
            synchronize();
            getPointsValue(_t1.tv_sec);
            record();
            unsetPointInput();
            gettimeofday(&_t2, NULL);

            //清理记录ts返回为0的记录
            if (_timezero_liveids.size() > 5) {
                map<time_t, vector<int> >::iterator it = _timezero_liveids.end();
                --it;
                --it;
                _timezero_liveids.erase(_timezero_liveids.begin(), it);
            }

            int usec = _t2.tv_usec - _t1.tv_usec;
            time_t sec = _t2.tv_sec - _t1.tv_sec;
            if (usec < 0) {
                usec += 1000000;
                --sec;
            }


            LOG() << debug << "Thread:" << (unsigned int) id() << " elapsed time:" << sec << "." << usec << endl;
            _elapsed_time = sec * 1000 + usec / 1000;
            if (sec < _intval_time) {
                usleep_time = 1000000 * (_intval_time - sec) - usec;
                usleep(usleep_time);
            }
        } while (_running);
    } catch (const TL_Exp &e) {
        LOG() << error << ":Exception:" << e.what() << endl;
    } catch (const std::exception &e) {
        LOG() << error << ":Exception:" << e.what() << endl;
    } catch (...) {
        LOG() << error << ":Exception:unknow." << endl;
    }
}

bool EdsPointThread::checkLiveClient() {
    if (_live_client == NULL) {
        LOG() << error << "Live Client is null." << endl;
        return false;
    }
    return true;
}

bool EdsPointThread::initLiveIds() {
    if (!checkLiveClient()) return false;
    LOG() << "init points.size= " << _points.size() << " begin." << endl;
    _liveid_pos.clear();
    for (int i = 0; i < _points.size(); ++i) {
        const string& point = _points[i];
        const string& wizid = _wizids[i];
        int liveid = _live_client->findByIESS(point.c_str());
        if (liveid == -1) {
            LOG() << warn << "Point " << point << " find live id failed." << endl;
            _noliveid_points.insert(point);
        } else {
            myPointCache& cache = _liveid_pos[liveid];
            cache.pos = i; //pos是来找string 名字的
            //_pos_liveid[i] = liveid;
            TL_ThreadLock::Lock lk(g_lock);
            pair<EdsPointThread*, int>& obj_id = _wizid_thread_liveid[wizid];
            obj_id.first = this;
            obj_id.second = liveid;
            _tgn_thread_liveid[point] = obj_id;
        }
    }
    return true;
}

void EdsPointThread::setPointInput() {
    if (!checkLiveClient()) return;
    map<int, myPointCache>::const_iterator it = _liveid_pos.begin();
    while (it != _liveid_pos.end()) {
        try {
            _live_client->setInput(it->first);
        } catch (const Error* exc) {
            LOG() << warn << "Failed to setInput for Point(" << _points[it->second.pos] << ") " << exc->what() << endl;
        }
        ++it;
    }
}

void EdsPointThread::unsetPointInput() {
    if (!checkLiveClient()) return;
    map<int, myPointCache>::const_iterator it = _liveid_pos.begin();
    while (it != _liveid_pos.end()) {
        try {
            _live_client->unsetInput(it->first);
        } catch (const Error* exc) {
            LOG() << warn << "Failed to unsetInput for Point(" << _points[it->second.pos] << ") " << exc->what() << endl;
        }
        ++it;
    }
}

bool EdsPointThread::synchronize() {
    if (!checkLiveClient()) return false;
    try {
        do {
            _live_client->synchronizeInput();
        } while (_live_client->isUpdateRequired());
    } catch (const Error& exc) {
        LOG() << warn << "Failed to synchronize input point value " << exc.what() << endl;
        return false;
    }
    return true;
}

SwitchData::SwitchNode* EdsPointThread::findSwitchNode(const string&tgn) {
    map<string, SwitchData::SwitchNode*>::iterator it = _switch_data_tmp_cache.find(tgn);
    if (it != _switch_data_tmp_cache.end()) {
        return it->second;
    } else {
        SwitchData::SwitchNode* node = _switch_data->findSwitchNode(tgn);
        if (node) {
            _switch_data_tmp_cache[tgn] = node;
            return node;
        }
    }
    _switch_data_tmp_cache[tgn] = NULL;
    return NULL;
}

void EdsPointThread::checkSwitchData(const string& tgn, time_t t, int val) {
    if (_switch_data) {
        SwitchData::SwitchNode* node = findSwitchNode(tgn);
        if (t == 0) {
            val = -1;
            t = _t1.tv_sec;
        }
        if (node) {
            int type = node->switchType(tgn);
            if (type == 1) {
                if (val != node->v1) {
                    //changed
                    node->t1 = t;
                    node->last_uptime = t;
                    node->v1 = val;
                    _changed_switch_nodes.insert(node);
                }
            } else if (type == 2) {
                if (val != node->v2) {
                    //changed
                    node->t2 = t;
                    node->last_uptime = t;
                    node->v2 = val;
                    _changed_switch_nodes.insert(node);
                }
            }
        }
    }
}

void EdsPointThread::allockNewBlcok(const string& point, myPointCache& cache) {
    if (cache.block_id < 0) {
        //create new block
        ++_block_id_max;
        cache.block_id = _block_id_max;
        int start_off = 0;
        TL_Mmap mp(false);
        pwrite(_datafile_fd, "\0", 1, (cache.block_id + 1) * EDSPOINTBUFFLEN - 1);
        mp.mmap(EDSPOINTBUFFLEN, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, _datafile_fd, cache.block_id * EDSPOINTBUFFLEN);
        cache.mmap_addr = (unsigned char *) mp.getPointer();
        cache.bufflen = (int *) cache.mmap_addr;
        cache.count = cache.bufflen + 1;
        cache.comp.clear();
        cache.comp.bbuf.buffer = cache.mmap_addr + 10;
        char tmp[64];
        sprintf(tmp, "\",%d,%d,0)", cache.block_id, start_off);
        string sql = "insert into t_tgn values(\"";
        sql += point;
        sql += tmp;
        LOG() << debug << __LINE__ << ":allockNewBlcok:bufflen:" << *cache.bufflen << "|off:" << start_off << ":" << sql << endl;
        sqlite3_exec(_sqlite3_handler, sql.c_str(), NULL, NULL, NULL);
    }
}

bool EdsPointThread::getLivePointValue(time_t t, int liveid) {
    if (!checkLiveClient()) return false;
    try {
        myPointCache& cache = _liveid_pos[liveid];
        if (cache.pos >= _points.size()) {
            LOG() << warn << "cache.pos=" << cache.pos << "," << cache.values.size() << " not found _points pos." << _points.size() << endl;
            return false;
        }
        int ts = _live_client->readFieldInt(liveid, "TS");
        char type = _live_client->pointRT(liveid);
        char quality;
        float value = _live_client->readAnalog(liveid, &quality);
        //switch

        const string& point = _points[cache.pos];

        checkSwitchData(point, ts, value);

        if (ts > 0) {
            {
                TL_ThreadRwLock::WLock wl(_rw_lock);
                myPointValue & pvalue = cache.values[ts];
                pvalue.type = type;
                pvalue.quality = quality;
                pvalue.ts = ts;
                if (type != 'B' && type != 'P') {
                    pvalue.value.f = value;
                } else {
                    pvalue.value.i = value;
                }
            }
            if (cache.values.size() > _save_data_size + 20) {
                //clear
                TL_ThreadRwLock::WLock wl(_rw_lock);
                time_t ts1 = ts - _intval_time * _save_data_size;
                std::map<int, myPointValue>::iterator it = cache.values.upper_bound(ts1);
                cache.values.erase(cache.values.begin(), it);
                //LOG() << debug << "Point Data clear:" << point << " cache.value.size=" << cache.values.size() << endl;
            }

            return true;
        } else {
            //todo controler getdata after 5 min later.
            //LOG() << debug << "Point=" << point << ",liveid=" << liveid << " TS is zero" << endl;
            _timezero_liveids[t].push_back(liveid);
            return false;
        }
    } catch (const Error& exc) {
        LOG() << warn << "Failed to get point value " << exc.what() << endl;
    }
    return false;
}

/**
 * 每个point一个文件,后期做合并,冷数据保存;避免管理的麻烦
 * @param filename
 * @param point
 * @param comp
 */

/**
void EdsPointThread::sync2File(const string& filename, const string& point, const myPointValue& pvalue, pointCompress& comp) {
    FILE * fp = fopen(filename.c_str(), "a+");
    if (!fp) {
        LOG() << error << "open " << filename << " failed" << endl;
        return;
    }
    int fd = fileno(fp);
    off_t flielen = TL_Common::getFileLength(fileno(fp));
    if (flielen == 0) {
        //new file
        int data_len = comp.bbuf.buffer - (unsigned char *) comp.buff_str.c_str();
        if (data_len > 0) {
            memset(_buffer, 0, 1024);
            memcpy(_buffer, point.c_str(), point.size());
            _buffer[62] = pvalue.type;
            _buffer[63] = pvalue.quality;
            compressBlock *block = (compressBlock*) (_buffer + 64);
            block->block_id = 1; //从1开始
            block->block_start_off = 1024;
            block->block_size = data_len;
            block->block_elm_count = comp.count;
            write(fd, _buffer, 1024);
            write(fd, comp.buff_str.c_str(), data_len);
            comp.file_no = 1;
            comp.last_write_pos = data_len;
        }
        fclose(fp);
        return;
    }
    //char buffer[1024];
    fread(_buffer, 1, 1024, fp);
    compressBlock *block = (compressBlock*) (_buffer + 64);
    int data_len = comp.bbuf.buffer - (unsigned char *) comp.buff_str.c_str();

    compressBlock *block2 = block;
    do {
        ++block2;
    } while (block2->block_id != 0);
    --block2; //last block



    if (block->block_id == comp.file_no) {
        //继续可用
        if (data_len >= comp.last_write_pos) {
            pwrite(fd,
                    comp.buff_str.c_str() + comp.last_write_pos - 1,
                    data_len - comp.last_write_pos + 1,
                    block2->block_start_off + comp.last_write_pos - 1);
            block->block_size = data_len;
            block->block_elm_count = comp.count;
            pwrite(fd, (char *) block2, sizeof (compressBlock), (char *) block2 - _buffer);
            comp.last_write_pos = data_len;
        }
    } else {
        //new block
        block = block2;
        ++block2;
        block2->block_id = block->block_id + 1;
        comp.file_no = block2->block_id;
        block2->block_start_off = block->block_start_off + block->block_size;
        block2->block_size = data_len;
        block2->block_elm_count = comp.count;

        pwrite(fd,
                comp.buff_str.c_str(),
                data_len,
                block2->block_start_off);
        pwrite(fd, (char *) block2, sizeof (compressBlock), (char *) block2 - _buffer);
        comp.last_write_pos = data_len;
    }
    fclose(fp);
}
 */
void EdsPointThread::setHistoryRootPath(const string& hrp) {
    history_root_path = hrp;
    if (history_root_path.empty()) history_root_path = "/tmp";
}

void EdsPointThread::getPointsValue(time_t t) {
    if (!synchronize()) {
        LOG() << warn << "LiveClient synchronize failed." << endl;
        return;
    }
    //按天来存储历史数据
    time_t t2 = (t + BEIJIN_TIME) / ONDAY_SECONDS * ONDAY_SECONDS;
    if (last_history_time_path != t2) {
        _block_id_max = -1;
        tidp::TL_Datetime dt(t);
        history_current_path = history_root_path + dt.toString("/%Y%m%d/");
        TL_Common::mkdirRecursive(history_current_path);

        //全局得考虑所有的同步情况,这是蛮复杂的逻辑,独立线程搞
        //TL_ThreadLock::Lock lk(SQLITE3Lock);

        if (_sqlite3_handler) sqlite3_close(_sqlite3_handler);
        if (_datafile_fd > 0) close(_datafile_fd);


        char tmp[64];
        sprintf(tmp, "tgn_%d", _thread_no);
        string sqldb = history_current_path + tmp + ".db";
        sqlite3_open(sqldb.c_str(), &_sqlite3_handler);

        string datafile = history_current_path + tmp + ".data";

        _datafile_fd = open(datafile.c_str(), O_CREAT | O_RDWR, 0666);
        if (_datafile_fd < 0) {
            LOG() << "open file :" << datafile << " fail:" << errno << ":" << strerror(errno) << endl;
            throw TL_Exp(datafile + " open failed.", errno);
        }
        string sql = "create table if not exists t_tgn(\
tgn varchar(64),\
block_id int,\
start_off int,\
begin_t bigint\
)";
        sqlite3_exec(_sqlite3_handler, sql.c_str(), NULL, NULL, NULL);
        //此处用于重启恢复数据
        sqlite3_stmt * stmt;
        sql = "select tgn,max(block_id),max(block_id * 4096 + start_off) from t_tgn group by tgn";
        sqlite3_prepare(_sqlite3_handler, sql.c_str(), sql.size(), &stmt, NULL);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string tgn = (char *) sqlite3_column_text(stmt, 0);
            //TL_ThreadLock::Lock lk(g_lock);
            int liveid = -1;
            {
                TL_ThreadLock::Lock lk(g_lock);
                map<string, pair<EdsPointThread*, int> >::const_iterator it = _tgn_thread_liveid.find(tgn);
                if (it != _tgn_thread_liveid.end()) {
                    liveid = it->second.second;
                    if (it->second.first != this) {
                        LOG() << error << "tgn:" << tgn << " not in this[" << (void *) this << "],but in " << (void*) it->second.first << endl;
                        //配置文件切换,导致原本的tgn不在原先的配置序号内;这里需要忽略。tgn转场
                        //live_id有效,但是已经转换了线程序号;todo 换正确的线程处理? 这里比较复杂了。
                        //bug fix@20170509 21
                        continue;
                    }
                } else {
                    LOG() << error << " not found " << tgn << " 's live id." << endl;
                    continue;
                }
            }
            int block_id = sqlite3_column_int(stmt, 1);
            int start_off = (sqlite3_column_int(stmt, 2) & (0x00000fff));
            if (block_id >= 0 && start_off >= 0 && EDSPOINTBUFFLEN - start_off > 64) {
                //判别剩余空间还有,值得再利用
                TL_Mmap mp(false);
                //pwrite(_datafile_fd, "\0", 1, (block_id + 1) * EDSPOINTBUFFLEN - 1);
                mp.mmap(EDSPOINTBUFFLEN, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, _datafile_fd, block_id * EDSPOINTBUFFLEN);
                //unsigned char * buffer = (unsigned char *) mp.getPointer();
                myPointCache& cache = _liveid_pos[liveid];
                cache.block_id = block_id;
                cache.mmap_addr = (unsigned char *) mp.getPointer();
                cache.bufflen = (int *) (cache.mmap_addr + start_off);
                cache.count = cache.bufflen + 1;
                int start_off_new = start_off;
                if (*cache.bufflen > 10 && *cache.bufflen < EDSPOINTBUFFLEN - start_off) {
                    if (EDSPOINTBUFFLEN - start_off < 32) {
                        cache.block_id = 0;
                        mp.setAutoDestroy(true);
                        continue;
                    }
                    //重新利用空间,启用新位置
                    start_off_new = start_off + *cache.bufflen;
                    char tmp[64];
                    sprintf(tmp, "\",%d,%d,0)", cache.block_id, start_off_new);
                    sql = "insert into t_tgn values(\"";
                    sql += tgn;
                    sql += tmp;
                    LOG() << debug << __LINE__ << ":" << *cache.bufflen << "|new off:" << start_off_new << "|" << sql << endl;
                    sqlite3_exec(_sqlite3_handler, sql.c_str(), NULL, NULL, NULL);
                }
                //else 重新利用此位置
                cache.bufflen = (int *) (cache.mmap_addr + start_off_new);
                cache.count = cache.bufflen + 1;
                *cache.bufflen = 10;
                cache.comp.clear();
                cache.comp.bbuf.buffer = cache.mmap_addr + start_off_new + 10;
            }
            if (_block_id_max < block_id) _block_id_max = block_id;
        }
        sqlite3_finalize(stmt);
        last_history_time_path = t2;
    }
    map<int, myPointCache>::const_iterator it = _liveid_pos.begin();
    while (it != _liveid_pos.end()) {
        getLivePointValue(t, it->first);
        ++it;
    }
}

void EdsPointThread::record() {
    map<int, myPointCache>::iterator it = _liveid_pos.begin();

    enum {
        buffer_size = 256
    };
    char buffer[buffer_size];
    int kafka_cache_id = 0;
    int l = 0;
    //string point_record;
    //_kafka_cache.clear();
    map<time_t, string> date_time_str_cache;
    TL_Datetime dt;
    while (it != _liveid_pos.end()) {
        myPointCache& cache = it->second;
        const string& point = _points[cache.pos];
        const string& wizid = _wizids[cache.pos];
        if (cache.values.size() > 0) {
            std::map<int, myPointValue>::value_type & v = *cache.values.rbegin();
            if (cache.last_record_ts < v.first) {

                const myPointValue& pvalue = v.second;
                if (pvalue.type != 'B' && pvalue.type != 'P') {
                    l = snprintf(buffer, buffer_size, "%f|%c", pvalue.value.f, pvalue.quality);
                } else {
                    l = snprintf(buffer, buffer_size, "%d|%c", pvalue.value.i, pvalue.quality);
                }
                //可以拆分线程去独立处理，提升速度
                string & dtstr = date_time_str_cache[v.first];
                if (dtstr.size() == 0) {
                    dt.setTime(v.first);
                    dtstr = dt.toString();
                }

                //dt.toString();
                if (_is_record_file) {
                    //DLOG("point") << noop << point << "|" << wizid << "|" << dtstr << "|" << buffer << endl;
                    LOG("point") << noop << point << "|" << wizid << "|" << dtstr << "|" << buffer << endl;
                }
                if (_is_record_kafka) {
                    string& point_record = _kafka_cache[kafka_cache_id];
                    point_record.clear();
                    point_record.assign(point);
                    point_record.append("|");
                    point_record.append(wizid);
                    point_record.append("|");
                    point_record.append(dtstr);
                    point_record.append("|");
                    point_record.append(buffer, l);
                    ++kafka_cache_id;
                    //_kafka_cache.push_back(point_record);
                }

                //compress ========================================================================
                pointCompress &pcomp = cache.comp;
                if (cache.block_id < 0) {
                    allockNewBlcok(point, cache);
                }

                if (cache.comp.bbuf.buffer - cache.mmap_addr + 32 > EDSPOINTBUFFLEN) {
                    cache.block_id = -1;
                    TL_Mmap::munmap((void *) cache.mmap_addr,
                            EDSPOINTBUFFLEN);
                    allockNewBlcok(point, cache);
                }
                time_t ts = v.first;
                if (pcomp.last_ts == 0) {
                    * cache.count = 0; //清0 如果文件中含有老的数据
                    char * c = (char *) (cache.count + 1);
                    *c = pvalue.type;
                    ++c;
                    *c = pvalue.quality;
                    BitCompress::compressInt(ts, pcomp.bbuf);

                    BitCompress::compressXORInt(pvalue.value.u, pcomp.bbuf, pcomp.bz);
                    pcomp.last_ts = ts;
                    pcomp.last_dealt_ts = 0;
                    pcomp.last_value.u = pvalue.value.u;
                    LOG() << "compress:" << point << "\t" << *cache.count << ":(" << ts << "," << pvalue.value.u << ")" << endl;
                } else {
                    int delat = ts - pcomp.last_ts;
                    BitCompress::compressInt(delat - pcomp.last_dealt_ts, pcomp.bbuf);
                    pcomp.last_dealt_ts = delat;

                    BitCompress::compressXORInt(pcomp.last_value.u xor pvalue.value.u, pcomp.bbuf, pcomp.bz);
                    LOG() << "compress:" << point << "\t" << *cache.count << ":(" << ts << "," << pvalue.value.u << ")("
                            << (delat - pcomp.last_dealt_ts) << "," << (pcomp.last_value.u xor pvalue.value.u) << ")" << endl;
                    pcomp.last_value.u = pvalue.value.u;
                }
                ++(*cache.count);
                *cache.bufflen = pcomp.bbuf.buffer - (unsigned char *) cache.bufflen + 1;
                //end compress, add by ever 20170410
                //=================================================================================
                cache.last_record_ts = v.first;
            }
        }
        ++it;
    }

    if (_is_record_kafka && kafka_cache_id > 0) {
        //_kafka_cache.resize(kafka_cache_id);
        //LOG() << debug << "write2Kafka:" << _kafka_cache.size() << endl;
        _kafka_svr->write2Kafka(_topicname, _kafka_cache, kafka_cache_id, _partition_num, WRITE_KAFKA_TYPE_POLL);
    }

    recordSwitchData();
}

void EdsPointThread::recordSwitchData() {
    if (!_is_record_kafka) {
        return;
    }
    set<SwitchData::SwitchNode*>::iterator it = _changed_switch_nodes.begin();
    int kafka_cache_id = 0;

    enum {
        tmpsize = 256
    };
    char tmp[tmpsize];
    while (it != _changed_switch_nodes.end()) {
        SwitchData::SwitchNode* node = *it;
        string& switch_record = _kafka_cache[kafka_cache_id];

        switch_record.assign(node->eqid);
        switch_record.append("|");
        switch_record.append(node->tgn1);
        //switch_record.append(node->cn1);
        //switch_record.append("|");
        int i = snprintf(tmp, tmpsize, "|%lld|%lld|", node->/*t1*/last_uptime, node->v1);
        switch_record.append(tmp, i);

        switch_record.append(node->tgn2);
        //switch_record.append("|");
        //switch_record.append(node->cn2);
        //switch_record.append("|");
        i = snprintf(tmp, tmpsize, "|%lld|%lld", node->/*t2*/last_uptime, node->v2);
        switch_record.append(tmp, i);
        ++kafka_cache_id;
        ++it;
    }
    _kafka_svr->write2Kafka(_switch_topic_name, _kafka_cache, kafka_cache_id, _switch_partition_num, WRITE_KAFKA_TYPE_POLL);
}

void EdsPointThread::setInterval(int intval) {
    _intval_time = intval;
    if (_intval_time <= 0) _intval_time = 1;
}

void EdsPointThread::setThreadNo(int tno) {
    _thread_no = tno;
}

int EdsPointThread::getThreadNo() {
    return _thread_no;
}

void EdsPointThread::setKafkaServer(bool iskafka, KafkaServer * kafka_svr, const string & topic, int partition) {
    _is_record_kafka = iskafka;
    _kafka_svr = kafka_svr;
    _topicname = topic;
    _partition_num = partition;
}

bool EdsPointThread::getPointValuesFromCache(int liveid, std::map<int, myPointValue>& values) {
    TL_ThreadRwLock::RLock rl(_rw_lock);
    map<int, myPointCache>::const_iterator it = _liveid_pos.find(liveid);
    if (it == _liveid_pos.end()) return false;
    values = it->second.values; //copy
    return true;
}

size_t EdsPointThread::getValidIdSize() {
    return _liveid_pos.size();
}

size_t EdsPointThread::getInvalidIdSize() {
    //无效id数
    return _noliveid_points.size();
}

string EdsPointThread::status2Json() {
    ostringstream os;
    os << "{file:'";
    os << _point_file << "',threadAddr:'" << (void*) this << "',threadNo:" << _thread_no << ",intval:" << _intval_time << ",validIdSize:";
    os << getValidIdSize() << ",InvalidIdSize:"; //有效点
    os << getInvalidIdSize() << ",ZeroTsSize:"; //无效点
    if (_timezero_liveids.size() > 0) {
        os << _timezero_liveids.rbegin()->second.size(); //TS 为0点
    } else {
        os << "'no data'";
    }
    os << ",elapsedTime:" << _elapsed_time;
    os << "}";
    return os.str();
}

void EdsPointThread::zeroTsIds2Json(ostream & os) {
    os << "{file:'" << _point_file << "',";
    os << "zerotgn:[";
    if (_timezero_liveids.size() > 0) {
        const vector<int>& zvs = _timezero_liveids.rbegin()->second;

        vector<int>::const_iterator it = zvs.begin();
        vector<int>::const_iterator end = zvs.end();
        --end;
        int i = 0;
        while (it != end) {
            os << "'" << _points[_liveid_pos[*it].pos] << "',";
            ++it;
            if (++i % 20 == 0) {
                os << "\n";
            }
            //if(it!=)
        }
        if (it != zvs.end()) {
            os << "'" << _points[_liveid_pos[*it].pos] << "'";
        }
    }
    os << "]}";
}

void EdsPointThread::history(ostream& os, const string& tgn, time_t stime, time_t etime) {
    sqlite3 * db;
    int data_fd = -1;
    do {
        if ((stime + BEIJIN_TIME) / ONDAY_SECONDS == (_t1.tv_sec + BEIJIN_TIME) / ONDAY_SECONDS) {
            //today
            db = _sqlite3_handler;
            data_fd = _datafile_fd;
        } else {
            //TL_Datetime dt((stime + BEIJIN_TIME) / ONDAY_SECONDS * ONDAY_SECONDS);
            TL_Datetime dt(stime);
            string dbfile = history_root_path + dt.toString("/%Y%m%d/");
            char tmp[64];
            sprintf(tmp, "tgn_%d.db", _thread_no);
            dbfile += tmp;
            int ret = sqlite3_open_v2(dbfile.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
            if (ret != SQLITE_OK) {
                os << "{retcode:-1,retmsg=\"" << sqlite3_errmsg(db) << ":" << dbfile << "\"}";
                break;
            }
            sprintf(tmp, "tgn_%d.data", _thread_no);
            string datafile = history_root_path + dt.toString("/%Y%m%d/") + tmp;
            data_fd = open(datafile.c_str(), O_RDWR);
            if (data_fd < 0) {
                os << "{retcode:-1,retmsg=\"data file " << datafile << " open faild.\"}";
                break;
            }
        }

        string sql = "select * from t_tgn where tgn=\"" + tgn + "\"";
        sqlite3_stmt * stmt;
        sqlite3_prepare(db, sql.c_str(), sql.size(), &stmt, NULL);
        vector<pair<int, int> > vtmp; //block_id,offset
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            vtmp.push_back(make_pair(
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_int(stmt, 2)
                    ));
        }
        sqlite3_finalize(stmt);

        if (vtmp.size() == 0) {
            os << "{retcode:-1,retmsg=\"no block info.\"}";
            break;
        }
        char type, quality;
        vector<pair<time_t, value_t > > values;
        for (int i = 0; i < vtmp.size(); ++i) {
            const pair<int, int>& blkoff = vtmp[i];
            TL_Mmap mp;
            mp.mmap(EDSPOINTBUFFLEN, PROT_READ, MAP_SHARED | MAP_FILE, data_fd, blkoff.first * EDSPOINTBUFFLEN);
            unsigned char * base_addr = (unsigned char *) mp.getPointer();
            int * len = (int *) (base_addr + blkoff.second);
            int * count = len + 1;
            type = *((char *) (count + 1));
            quality = *((char *) (count + 1) + 1);

            BitCompress::BitBuffer bbuf;
            BitCompress::BitZero bz;
            bbuf.buffer = (unsigned char *) (base_addr + blkoff.second + 10);
            bbuf.pos = 0;
            bz.clz = 0;
            bz.cnz = 32;

            time_t last_tm = 0;
            value_t last_v;

            unsigned int bxor = 0;
            int last_dt = 0;
            if (*count > 0) { //初值
                last_tm = BitCompress::uncompressInt(bbuf);
                last_v.u = BitCompress::uncompressXORInt(bbuf, bz);
            }
            if (last_tm > etime) {
                //continue;
                break;
            }
            if (last_tm >= stime) {
                values.push_back(make_pair(last_tm, last_v));
            }
            int k = 0;

            //unsigned char * buf_start = (unsigned char *) mp.getPointer() + 10;

            //int bit1, bit2, bit3;
            for (int i = 1; i < *count; ++i) {
                //bit1 = (bbuf.buffer - buf_start)*8L + bbuf.pos;
                //cout << i << "\t" << bit1 << " (" << bbuf.buffer - buf_start << "," << (int) bbuf.pos << ") ";
                k = BitCompress::uncompressInt(bbuf);
                last_tm += k + last_dt;
                last_dt = k;
                //bit2 = (bbuf.buffer - buf_start)*8L + bbuf.pos;
                //cout << (bit2 - bit1) << "," << bit2 << " (" << bbuf.buffer - buf_start << "," << (int) bbuf.pos << ") ";
                bxor = BitCompress::uncompressXORInt(bbuf, bz);
                //bit3 = (bbuf.buffer - buf_start)*8L + bbuf.pos;
                //cout << (bit3 - bit2) << "," << bit3 << " (" << bbuf.buffer - buf_start << "," << (int) bbuf.pos << ")," << bxor;
                last_v.u ^= bxor;
                if (last_tm > etime) {
                    break;
                }
                if (last_tm >= stime) {
                    values.push_back(make_pair(last_tm, last_v));
                }
            }
            if (last_tm > etime) {
                break;
            }
        }

        if (values.size() > 0) {
            os << "{retcode:0,type:\"" << type << "\",quality:\"" << quality << "\",tgn:\"" << tgn << "\",";
            TL_Datetime dt(stime);
            os << "range:\"" << dt.toString() << "|";
            dt.setTime(etime);
            os << dt.toString() << "\",";

            vector<pair<time_t, value_t > >::iterator it = values.begin();
            time_t stt = it->first;
            os << "stime:" << stt << ",data:[0,";

            if (type != 'B' && type != 'P') {
                //float
                os << it->second.f;
            } else {
                os << it->second.i;
            }
            ++it;
            while (it != values.end()) {
                os << "," << it->first - stt << ",";
                if (type != 'B' && type != 'P') {
                    //float
                    os << it->second.f;
                } else {
                    os << it->second.i;
                }
                ++it;
            }
            os << "]}";
        } else {
            os << "{retcode:-1,retmsg=\"no data in block\"}";
            break;
        }
    } while (0);
    //clean
    if (data_fd > 0 && data_fd != _datafile_fd) {
        close(data_fd);
    }
    if (db != _sqlite3_handler) {
        sqlite3_close(db);
    }
}


