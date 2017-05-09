/* 
 * File:   main.cpp
 * Author: ever
 *
 * Created on 2016年4月16日, 下午8:57
 */

#include <cstdlib>
#include <iostream>
#include "TL_Option.h"
#include "TL_IniFile.h"
#include "TL_Logger.h"
#include "EdsLiveData.h"
#include "EdsPointThread.h"
#include "PointData.h"
#include "sys/time.h"
#include "KafkaServer.h"
#include "HttpServer.h"
using namespace std;
using namespace tidp;

/*
 * 
 */
void usage(const char * pname) {
    cout << "Usage:" << pname << " "
            "--conf=configure file.\n"
            //"--pointfile=point file\n"
            << endl;
}

int main(int argc, char** argv) {
    try {

        cout << "starting........................." << endl;
        TL_Logger* _logger = TL_Logger::getInstance();

        TL_Option opt;
        opt.decode(argc, argv);
        if (!opt.hasParam("conf")) {
            usage(argv[0]);
            return 0;
        }

        //string conf = opt.getValue("conf");
        TL_IniFile ini(opt.getValue("conf"));
        _logger->init(ini["Login.LogPath"], 10, (1UL << 30));
        _logger->setPeriodInfo(ini["Login.LogPath"], 10 * 60);
        _logger->start();
        LOG() << "EdsData Serve Starting ............................................." << endl;
        string eds_live_host = ini["Login.Host"];
        int eds_live_port = atoi(ini["Login.LivePort"].c_str());
        //int eds_arch_port = atoi(ini["Login.ArchPort"].c_str());
        string eds_version = ini["Login.Version"];

        //EdsLiveData eds_live;


        map<pair<int, string>, PointData*> _time_for_points;
        string eds_pointconf = ini["Point.files"];
        vector<string> files;
        TL_Common::split(files, eds_pointconf, ";");
        if (files.size() == 0) {
            cerr << "not set point files." << endl;
            //return 0;
        }
        vector<string> filetime;
        for (int i = 0; i < files.size(); ++i) {
            TL_Common::split(filetime, files[i], ",");
            int intervaltime = 1; //default
            if (filetime.size() > 1) {
                intervaltime = atoi(filetime[1].c_str());
                if (intervaltime < 1) {
                    intervaltime = 1;
                }
            }
            if (filetime.size() > 0) {
                pair<int, string> key = make_pair(intervaltime, filetime[0]);

                if (_time_for_points.find(key) == _time_for_points.end()) {
                    _time_for_points[key] = new PointData;
                }

                try {
                    _time_for_points[key]->loadFromPointFile(key.second, true);
                } catch (const exception& e) {
                    LOG() << "loadFromPointFile:" << e.what() << endl;
                }
            }
        }
        //switch init 启停相关设置
        bool switchEnable = false;
        string switchFile = TL_Common::trim(ini["Switch.file"]);
        SwitchData switchData;
        if (switchFile.size() > 0) {
            switchData.loadFromFile(switchFile);
            switchEnable = true;
        }

        //kafka init
        bool kafkaEnable = (ini["Kafka.kafkaEnable"] == "true");

        string switchTopicName = ini["Kafka.switchTopicName"];
        int switchPartitionNum = atoi(ini["Kafka.switchPartitionNum"].c_str());
        if (switchPartitionNum < 0) switchPartitionNum = -1;
        int partition_num = -1;
        string topicName = ini["Kafka.topicName"];

        KafkaConf kf_conf;
        kf_conf.maxBuffer = 100000;
        kf_conf.batchNums = 1000;
        kf_conf.copmType = -1;
        kf_conf.isCallback = 0;
        kf_conf.retries = 2;
        kf_conf.brokerList = ini["Kafka.brokerList"];


        if (kafkaEnable) {
            partition_num = atoi(ini["Kafka.partitionNum"].c_str());
            if (partition_num == 0) partition_num = -1;
            LOG() << "Kafka enabled: brokerList:" << kf_conf.brokerList << "topic:" << topicName << " partition_num:" << partition_num << endl;
        }

        map<pair<int, string>, PointData*>::iterator it = _time_for_points.begin();
        int cid = 0;
        int tno = 0;
        while (it != _time_for_points.end()) {
            LiveClient * client = EdsLiveData::initializeClient(eds_version.c_str(), eds_live_host.c_str(), eds_live_port);

            EdsPointThread * edsthread = new EdsPointThread(client, it->second->getPoints(), it->second->getWizIds());
            if (kafkaEnable) {
                char tmp[32];
                snprintf(tmp, 32, "%d", ++cid);
                kf_conf.clientId = "eds_produce_";
                kf_conf.clientId.append(tmp);
                KafkaServer *kafkasvr = new KafkaServer(kf_conf);
                kafkasvr->initialize();
                edsthread->setKafkaServer(kafkaEnable, kafkasvr, topicName, partition_num);
            }
            if (switchEnable) {
                edsthread->setSwitchData(&switchData, switchTopicName, switchPartitionNum);
            }
            LOG() << debug << "start thread for :" << it->first.second << endl;
            edsthread->setPointFileName(it->first.second);
            edsthread->setInterval(it->first.first);
            edsthread->start();
            edsthread->setThreadNo(++tno);
            edsthread->getControl().detach();
            edsthread->setHistoryRootPath(ini["History.store_path"]);
            _eds_threads.push_back(edsthread);
            ++it;
        }
        int i = 2;
        struct timeval t1, t2;
        do {
            gettimeofday(&t1, NULL);
            //eds_live.initLiveIds(points.getData(0));
            //eds_live.setPointInput();
            //eds_live.getPointsValue();
            //eds_live.unsetPointInput();
            gettimeofday(&t2, NULL);

            int usec = t2.tv_usec - t1.tv_usec;
            time_t sec = t2.tv_sec - t1.tv_sec;
            if (usec < 0) {
                usec += 1000000;
                --sec;
            }
            LOG() << debug << "elapsed time:" << sec << "." << usec << endl;
            if (sec == 0) {
                usleep(1000000 - usec);
            }
        } while (--i);

        EdsHttpServer http;
        http.run(1, argv);

        _logger->getControl().join();
        return Application::EXIT_OK;
    } catch (const TL_Exp&e) {
        cout << "Exception:" << e.what() << endl;
    } catch (const Error& e) {
        cout << "Error:" << e.what() << endl;
    } catch (const exception& e) {
        cout << "StdException:" << e.what() << endl;
    } catch (...) {
        cout << "Unknow Exception." << endl;
    }

    return 0;
}

