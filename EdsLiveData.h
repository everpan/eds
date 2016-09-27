/* 
 * File:   EdsLiveData.h
 * Author: ever
 *
 * Created on 2016年4月23日, 下午10:07
 */

#ifndef EDSLIVEDATA_H
#define EDSLIVEDATA_H
#include <string>
#include <map>
#include <vector>

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "LiveClient.h"
#include "ArchClient.h"
#include "TL_Logger.h"

#include "PointData.h"

using namespace std;
using namespace tidp;

using namespace eds;
using namespace eds::live;
using namespace eds::arch;



class EdsLiveData {
public:
    EdsLiveData();
    EdsLiveData(const EdsLiveData& orig);
    virtual ~EdsLiveData();


    bool initLiveIds(const vector<string>& points);
    //实时
    static LiveClient* initializeClient(const char* version,
            const char* host,
            unsigned short port);
    //历史
    ArchClient* initializeArchClient(const char* version,
            const char* host,
            unsigned short port);
    bool checkLiveClient();
    bool checkArchClient();
    void setPointInput();
    void unsetPointInput();
    bool synchronize();
    /**
     * 获取测点值，并记录文件
     */
    void getPointsValue();
protected:
    void getLivePointValues(int liveid);
private:
    LiveClient * _live_client;
    ArchClient * _arch_client;
    map<int, string> _live_ids_name;
    vector<int> _live_ids;
};

#endif /* EDSLIVEDATA_H */

