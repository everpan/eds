/* 
 * File:   PointData.h
 * Author: ever
 *
 * Created on 2016年4月16日, 下午9:50
 */

#ifndef POINTDATA_H
#define POINTDATA_H
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <vector>
#include "TL_Common.h"
#include "TL_Logger.h"
using namespace std;
using namespace tidp;

class PointDataException : public exception {
public:

    PointDataException() throw () {
    };
    PointDataException(const string& estr) throw ();
    virtual ~PointDataException() throw ();
    virtual const char* what() const throw ();
private:
    string _estr;
};

class PointData {
public:
    PointData();
    void loadFromPointFile(const string& point_file, bool append = false);
    virtual ~PointData();

    const vector<string>& getPoints();
    const vector<string>& getWizIds();
protected:
    PointData(const PointData& orig);
private:
    //map<int,int> _liveid_pos;
    vector<string> _point;
    vector<string> _wizid;
};

class SwitchData {
public:

    struct SwitchNode {
        string tgn1;
        string tgn2;
        string cn1;
        string cn2;
        int v1;
        int v2;
        time_t t1;
        time_t t2;
        time_t last_uptime;
        const bool operator<(const SwitchNode& other) {
            return tgn1 < other.tgn1 || (tgn1 == other.tgn1 && tgn2 < other.tgn2);
        }

        int switchType(const string& tgn) {
            if (tgn == tgn1) return 1;
            if (tgn == tgn2) return 2;
            return 0;
        }
        SwitchNode(const string & t1,const string & t2,const string & c1,const string &c2){
            tgn1 = t1;
            tgn2 = t2;
            cn1 = c1;
            cn2 = c2;
            v1 = v2 = 0;
            last_uptime = 0;
        }
    };
    SwitchNode* findSwitchNode(const string& tgn){
        map<string,SwitchNode*>::iterator it = _index.find(tgn);
        if(it != _index.end()) return it->second;
        return NULL;
    }
    
    void loadFromFile(const string& filename);
private:
    map<string,SwitchNode*> _index;
    set<SwitchNode*> _nodes;
};

#endif /* POINTDATA_H */

