/* 
 * File:   EdsLiveData.cpp
 * Author: ever
 * 
 * Created on 2016年4月23日, 下午10:07
 */

#include <memory>

#include "EdsLiveData.h"

EdsLiveData::EdsLiveData() {

}

EdsLiveData::EdsLiveData(const EdsLiveData& orig) {
    _live_client = NULL;
    _arch_client = NULL;
}

EdsLiveData::~EdsLiveData() {
    if (_live_client) {
        _live_client->shut();
        delete _live_client;
        _live_client = NULL;
    }
}

bool EdsLiveData::initLiveIds(const vector<string>& points) {
    if (!checkLiveClient()) return false;
    LOG() << "init points(" << points.size() << ") begin." << endl;
    _live_ids_name.clear();
    _live_ids.clear();
    vector<string>::const_iterator it = points.begin();
    while (it != points.end()) {
        int liveid = _live_client->findByIESS(it->c_str());
        if (liveid == -1) {
            LOG() << warn << "Point(" << *it << ") find leve id failed." << endl;
        } else {
            _live_ids_name[liveid] = *it;
            _live_ids.push_back(liveid);
        }
        ++it;
    }
}

LiveClient* EdsLiveData::initializeClient(const char* version,
        const char* host,
        unsigned short port) {
    try {
        LOG() << debug << "Creating LiveClient for EDS version " << version << endl;
        std::auto_ptr<LiveClient> client(new LiveClient(version));

        LOG() << debug << "Initializing LiveClient connection to " << host << ":" << port << endl;
        client->init(AccessMode_ReadWrite,
                "0.0.0.0", // bind to all local interfaces
                0, // bind to any local port
                host,
                port,
                50);

        LOG() << "LiveClient initialized successfully!" << endl;
        return client.release();
        //return _live_client;
    } catch (const BackendNotFoundError& exc) {
        LOG() << debug << "Couldn't load backend library for EDS " <<
                version << " " << exc.what() << endl;
        return 0;
    } catch (const LiveClientError& exc) {
        LOG() << debug << "Couldn't initialize client connection object " <<
                exc.what() << endl;
        return 0;
    }
}

ArchClient* EdsLiveData::initializeArchClient(const char* version,
        const char* host,
        unsigned short port) {
    try {
        LOG() << "Creating ArchClient for EDS version " << version << endl;
        std::auto_ptr<ArchClient> client(new ArchClient(version));

        // Initialize as client (read mode)
        LOG() << "Initializing ArchClient connection to " << host << ":" << port << endl;
        client->init("0.0.0.0", // bind to all local interfaces
                0, // bind to any local port
                host,
                port,
                50);

        LOG() << "ArchClient initialized successfully!" << endl;
        return client.release();
    } catch (const BackendNotFoundError& exc) {
        LOG() << "Couldn't load arch backend library for EDS " << version << "|" << exc.what() << endl;
        return 0;
    } catch (const ArchClientError& exc) {
        LOG() << "Couldn't initialize arch client connection object " << exc.what() << endl;
        return 0;
    }
}

bool EdsLiveData::checkLiveClient() {
    if (_live_client == NULL) {
        LOG() << debug << __FUNCTION__ << " failed." << endl;
        //todo reconnect
        return false;
    }
    return true;
}

bool EdsLiveData::checkArchClient() {
    if (_arch_client == NULL) {
        LOG() << debug << __FUNCTION__ << " failed." << endl;

        return false;
    }
    return true;
}

void EdsLiveData::setPointInput() {
    if (!checkLiveClient()) return;
    for (size_t i = 0; i < _live_ids.size(); ++i) {
        try {
            _live_client->setInput(_live_ids[i]);
        } catch (const Error* exc) {
            LOG() << warn << "Failed to setInput for Point(" << _live_ids_name[_live_ids[i]] << ") " << exc->what() << endl;
        }
    }
}

void EdsLiveData::unsetPointInput() {
    if (!checkLiveClient()) return;
    for (size_t i = 0; i < _live_ids.size(); ++i) {
        try {
            _live_client->unsetInput(_live_ids[i]);
        } catch (const Error* exc) {
            LOG() << warn << "Failed to unsetInput for Point(" << _live_ids_name[_live_ids[i]] << ") " << exc->what() << endl;
        }
    }
}

bool EdsLiveData::synchronize() {
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

void EdsLiveData::getPointsValue() {
    //if (!checkLiveClient()) return;
    bool b = synchronize();
    if (!synchronize()) {
        LOG() << warn << "LiveClient synchronize failed." << endl;
        return;
    }

    for (size_t i = 0; i < _live_ids.size(); ++i) {
        getLivePointValues(_live_ids[i]);
    }
}

void EdsLiveData::getLivePointValues(int liveid) {
    if (!checkLiveClient()) return;
    try {
        int ts = _live_client->readFieldInt(liveid, "TS");
        char type = _live_client->pointRT(liveid);
        char quality;
        float value = _live_client->readAnalog(liveid, &quality);
        char buffer[260];
        memset(buffer, 0, sizeof (buffer));
        DLOG("Point") << _live_ids_name[liveid] << "|" << value << ts << endl;
        if (type != 'B' && type != 'P') {
            DLOG("Point") << _live_ids_name[liveid] << "|" << value << ts << endl;
            //sprintf(buffer, "%s|%f|%d#", maps[lid].c_str(), value, ts);
        } else {
            //sprintf(buffer, "%s|%d|%d#", maps[lid].c_str(), value, ts);
            DLOG("Point") << _live_ids_name[liveid] << "|" << (int) value << ts << endl;
        }

        //return std::string(buffer);
    } catch (const Error& exc) {
        LOG() << warn << "Failed to get point value " << exc.what() << endl;
    }
}



