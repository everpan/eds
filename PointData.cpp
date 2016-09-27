/* 
 * File:   PointData.cpp
 * Author: ever
 * 
 * Created on 2016年4月16日, 下午9:50
 */

#include "PointData.h"

PointDataException::~PointDataException() throw () {
}

PointDataException::PointDataException(const string& estr) throw () {
    _estr = estr;
}

const char* PointDataException::what() const throw () {
    return _estr.c_str();
}

PointData::PointData() {
}

/**
 * 从文件
 * @param group
 * @param point_file
 */
void PointData::loadFromPointFile(const string& point_file,bool append) {
    std::ifstream file;
    file.open(point_file.c_str(), std::ios_base::in);
    if (!file.is_open()) {
        throw PointDataException(std::string("PointData::loadFromPointFile:Can't open ") + point_file);
    }
    if(!append){
        _point.clear();
        _wizid.clear();
    }
    string line;
    vector<string> fields;
    while (!file.eof()) {
        getline(file, line);
        TL_Common::split(fields, line, "|,");
        if (fields.size() > 1){
            _point.push_back(fields[0]);
            _wizid.push_back(fields[1]);
        }
    }
    LOG() << debug << __FUNCTION__ << " " << _point.size() << "/" << _wizid.size() << endl;
}

PointData::~PointData() {
}


const vector<string>& PointData::getPoints(){
    return _point;
}
const vector<string>& PointData::getWizIds(){
    return _wizid;
}



//------------------------------------------------------

void SwitchData::loadFromFile(const string& filename){
    std::ifstream file;
    file.open(filename.c_str(), std::ios_base::in);
    if (!file.is_open()) {
        throw PointDataException(std::string("SwitchData::loadFromFile:Can't open ") + filename);
    }
    
    string line;
    vector<string> fields;
    
    while (!file.eof()) {
        getline(file, line);
        TL_Common::split(fields, line, "|,");
        if (fields.size() > 3){
            SwitchNode* node = new SwitchNode(fields[0],fields[1],fields[2],fields[3]);
            
            _nodes.insert(node);
            //SwitchNode& n = const_cast<SwitchNode*>(*(_nodes.find(node)));
            if(node->tgn1.size() > 0)
                _index[node->tgn1] = node;
            if(node->tgn2.size() >0)
                _index[node->tgn2] = node;
        }
    }
}

