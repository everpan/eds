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

EdsPointThread::EdsPointThread(LiveClient * live_client, const vector<string> & points, const vector<string> & wizids) :
_live_client(live_client),
_points(points), _wizids(wizids) {
    _is_record_file = true;
    _is_record_kafka = false;
    _save_data_size = 3600; //保留最近x个数据
    _intval_time = 1; //默认1s采集一次
    _kafka_cache.resize(points.size());
    if (!checkLiveClient()) {
        LOG() << error << "EdsPointThread: init is null" << endl;
        return;
    }
    _switch_data = NULL;
}
void EdsPointThread::setPointFileName(const string& fname){
    _point_file = fname;
}
void EdsPointThread::setSwitchData(SwitchData * switchData,const string& switch_topic_name,int parttionNum){
    _switch_data = switchData;
    _switch_topic_name = switch_topic_name;
    _switch_partition_num = parttionNum;
}
EdsPointThread::~EdsPointThread() {
}

void EdsPointThread::run() {
    unsigned long long usleep_time;
    initLiveIds();
    _elapsed_time = 0;
    do {
        gettimeofday(&_t1, NULL);
        setPointInput();
        synchronize();
        getPointsValue(_t1.tv_sec);
        record();
        unsetPointInput();
        gettimeofday(&_t2, NULL);

        //清理记录ts返回为0的记录
        if(_timezero_liveids.size() > 5){
            map<time_t,vector<int> >::iterator it = _timezero_liveids.end();
            --it; --it;
            _timezero_liveids.erase(_timezero_liveids.begin(),it);
        }
        
        int usec = _t2.tv_usec - _t1.tv_usec;
        time_t sec = _t2.tv_sec - _t1.tv_sec;
        if (usec < 0) {
            usec += 1000000;
            --sec;
        }


        LOG() << debug << "Thread:" << (unsigned int) id() << " elapsed time:" << sec << "." << usec << endl;
        _elapsed_time = sec * 1000 + usec/1000;
        if (sec < _intval_time) {
            usleep_time = 1000000 * (_intval_time - sec) - usec;
            usleep(usleep_time);
        }
    } while (_running);

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
            _noliveid_points.push_back(point);
        } else {
            myPointCache& cache = _liveid_pos[liveid];
            cache.pos = i;
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
        LOG() << "Failed to synchronize input point value " << exc.what() << endl;
        return false;
    }
    return true;
}
SwitchData::SwitchNode* EdsPointThread::findSwitchNode(const string&tgn){
    map<string,SwitchData::SwitchNode*>::iterator it = _switch_data_tmp_cache.find(tgn);
    if(it != _switch_data_tmp_cache.end()){
        return it->second;
    }else{
        SwitchData::SwitchNode* node = _switch_data->findSwitchNode(tgn);
        if(node){
            _switch_data_tmp_cache[tgn] = node;
            return node;
        }
    }
    _switch_data_tmp_cache[tgn] = NULL;
    return NULL;
}
void EdsPointThread::checkSwitchData(const string& tgn,time_t t,int val){
    if(_switch_data){
        SwitchData::SwitchNode* node = findSwitchNode(tgn);
        if(t==0) val = -1;
        if(node){
            int type = node->switchType(tgn);
            if(type == 1){
                if(val != node->v1){
                    //changed
                    node->t1 = t;
                    node->last_uptime = _t1.tv_sec;
                    _changed_switch_nodes.insert(node);
                }
            }else if(type == 2){
                if(val != node->v2){
                    //changed
                    node->t2 = t;
                    node->last_uptime = _t1.tv_sec;
                    _changed_switch_nodes.insert(node);
                }
            }
        }
    }
}

bool EdsPointThread::getLivePointValue(time_t t,int liveid) {
    if (!checkLiveClient()) return false;
    try {
        myPointCache& cache = _liveid_pos[liveid];
        //const string& point = _points[cache.pos];

        int ts = _live_client->readFieldInt(liveid, "TS");
        char type = _live_client->pointRT(liveid);
        char quality;
        float value = _live_client->readAnalog(liveid, &quality);
        //switch
        
        const string& point = _points[cache.pos];
        
        checkSwitchData(point,ts,value);

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

void EdsPointThread::getPointsValue(time_t t) {
    if (!synchronize()) {
        LOG() << warn << "LiveClient synchronize failed." << endl;
        return;
    }
    map<int, myPointCache>::const_iterator it = _liveid_pos.begin();
    while (it != _liveid_pos.end()) {
        getLivePointValue(t,it->first);
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
    int l =0;
    //string point_record;
    //_kafka_cache.clear();
    map<time_t,string> date_time_str_cache;
    TL_Datetime dt;
    while (it != _liveid_pos.end()) {
        myPointCache& cache = it->second;
        const string& point = _points[cache.pos];
        const string& wizid = _wizids[cache.pos];
        if (cache.values.size() > 0) {
            std::map<int, myPointValue>::value_type & v = *cache.values.rbegin();
            if (cache.last_record_ts < v.first) {
                
                const myPointValue& pointv = v.second;
                if (pointv.type != 'B' && pointv.type != 'P') {
                    l = snprintf(buffer, buffer_size, "%f|%c", pointv.value.f, pointv.quality);
                } else {
                    l = snprintf(buffer, buffer_size, "%d|%c", pointv.value.i, pointv.quality);
                }
                //可以拆分线程去独立处理，提升速度
                string & dtstr = date_time_str_cache[v.first];
                if(dtstr.size() == 0){
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
                    point_record.append(buffer,l);
                    ++kafka_cache_id;
                    //_kafka_cache.push_back(point_record);
                }
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
void EdsPointThread::recordSwitchData(){
    set<SwitchData::SwitchNode*>::iterator it =  _changed_switch_nodes.begin();
    int kafka_cache_id = 0;
    while(it != _changed_switch_nodes.end()){
        SwitchData::SwitchNode* node = *it;
        //int i = snprintf();
        string& switch_record = _kafka_cache[kafka_cache_id];
        switch_record.assign(node->tgn1);
        switch_record.append("|");
        switch_record.append(node->cn1);
        switch_record.append("|");
        switch_record.append(node->t1);
        switch_record.append("|");
        switch_record.append(node->v1);
        switch_record.append("|");
        switch_record.append(node->tgn2);
        switch_record.append("|");
        switch_record.append(node->cn2);
        switch_record.append("|");
        switch_record.append(node->t2);
        switch_record.append("|");
        switch_record.append(node->v2);     
        ++ kafka_cache_id;
        ++it;
    }
    _kafka_svr->write2Kafka(_switch_topic_name, _kafka_cache, kafka_cache_id, _switch_partition_num, WRITE_KAFKA_TYPE_POLL);
}
void EdsPointThread::setInterval(int intval) {
    _intval_time = intval;
    if (_intval_time <= 0) _intval_time = 1;
}
void EdsPointThread::setThreadNo(int tno){
    _thread_no = tno;
}
int EdsPointThread::getThreadNo(){
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
string EdsPointThread::status2Json(){
    ostringstream os;
    os << "{file:'";
    os << _point_file << "',threadAddr:'" << (void*)this << "',threadNo:" << _thread_no << ",intval:" << _intval_time << ",validIdSize:";
    os << getValidIdSize() << ",InvalidIdSize:"; //有效点
    os << getInvalidIdSize() << ",ZeroTsSize:"; //无效点
    if(_timezero_liveids.size() > 0){
        os << _timezero_liveids.rbegin()->second.size(); //TS 为0点
    }else{
        os << "'no data'";
    }
    os << ",elapsedTime:" <<_elapsed_time;
    os << "}";
    return os.str();
}

void EdsPointThread::zeroTsIds2Json(ostream& os){
    os << "{file:'" << _point_file << "',";
    os << "zerotgn:[";
    if(_timezero_liveids.size() > 0){
        const vector<int>& zvs = _timezero_liveids.rbegin()->second;
        
        vector<int>::const_iterator it = zvs.begin();
        vector<int>::const_iterator end = zvs.end();
        --end;
        int i = 0;
        while(it != end){
            os << "'" << _points[_liveid_pos[*it].pos] << "',";
            ++it;
            if(++i%20 == 0){
                os << "\n";
            }
            //if(it!=)
        }
        if(it != zvs.end()){
            os << "'" << _points[_liveid_pos[*it].pos] << "'";
        }
    }
    os << "]}";
}


