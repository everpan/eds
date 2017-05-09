/* 
 * File:   HttpServer.cpp
 * Author: ever
 * 
 * Created on 2016年4月29日, 上午9:30
 */

#include "HttpServer.h"
#include "EdsPointThread.h"
#define DAY_SECONDS (24 * 60 * 60)
#define BEIJING_TIME_INV (8 * 60 * 60)

void EdsRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    std::ostream & ostr = response.send();
    const std::string& uri = request.getURI();
    if (uri.find("/Api/eds/wizids/") != std::string::npos) {
        try {
            std::vector<std::string> wizids;
            tidp::TL_Common::split(wizids, uri.substr(sizeof ("/Api/eds/wizids/") - 1), ",");
            //ostr << "wizids.size = " << wizids.size() << "<br/>\n";
            if (wizids.size() > 0) {
                ostr << "{retcode:0,data:[";
                std::map<int, myPointValue> values;
                std::vector<std::string>::const_iterator it = wizids.begin();
                while (it != wizids.end()) {
                    map<string, pair<EdsPointThread*, int> >::iterator it2 = _wizid_thread_liveid.find(*it);
                    if (it2 != _wizid_thread_liveid.end()) {
                        ostr << "{wizid:\"" << *it << "\",data:[";
                        if (it2->second.first->getPointValuesFromCache(it2->second.second, values)) {
                            if (values.size() > 0) {
                                std::map<int, myPointValue>::const_reverse_iterator it3 = values.rbegin();
                                int i = 0;
                                while (it3 != values.rend()) {
                                    const myPointValue& mpv = it3->second;
                                    ostr << "{ts:" << mpv.ts << ",type:'" << mpv.type << "',quality:'" << mpv.quality << "',value:";
                                    if (mpv.type != 'B' && mpv.type != 'P') {
                                        ostr << mpv.value.f;
                                    } else {
                                        ostr << mpv.value.i;
                                    }
                                    ostr << "}";
                                    if (++i > 5) break;
                                    ++it3;
                                    if (it3 != values.rend()) {
                                        ostr << ",";
                                    }
                                }
                            }
                        }
                        ostr << "]}";
                    } else {
                        ++it;
                        continue;
                    }
                    ++it;
                    if (it != wizids.end()) ostr << ",";
                }
            }
            ostr << "]}";
        } catch (const exception& e) {
            ostr << "{retcode:-1,retmsg=\"" << e.what() << "\"}\n";
        } catch (...) {
            ostr << "{retcode:-1,retmsg=\"unknow exception.\"}\n";
        }
    } else if (uri.find("/Api/eds/mtgn/") != std::string::npos) {
        try {
            std::vector<std::string> ps;
            tidp::TL_Common::split(ps, uri.substr(sizeof ("/Api/eds/mtgn/") - 1), "/");
            if (ps.size() > 1) {
                map<string, pair<EdsPointThread*, int> >::const_iterator it2 = _tgn_thread_liveid.find(ps[0]);
                if (it2 != _tgn_thread_liveid.end()) {
                    ostr << "{tgn:\"" << ps[0] << "\",data:[";
                    std::map<int, myPointValue> values;

                    int intval_min = 1;
                    if (ps[1].size()) {
                        intval_min = atoi(ps[1].c_str());
                    }
                    if (intval_min < 0) intval_min = 1;
                    time_t now = time(NULL);
                    now -= intval_min * 60;
                    if (it2->second.first->getPointValuesFromCache(it2->second.second, values)) {
                        if (values.size() > 0) {
                            std::map<int, myPointValue>::iterator it3 = values.upper_bound(now);
                            while (it3 != values.end()) {
                                const myPointValue& mpv = it3->second;
                                ostr << "{ts:" << mpv.ts << ",type:'" << mpv.type << "',quality:'" << mpv.quality << "',value:";
                                if (mpv.type != 'B' && mpv.type != 'P') {
                                    ostr << mpv.value.f;
                                } else {
                                    ostr << mpv.value.i;
                                }
                                ostr << "}";
                                ++it3;
                                if (it3 != values.end()) ostr << ",\n";
                            }
                        }
                    }
                    ostr << "]}";
                }
            } else {
                ostr << "{retcode:-1,retmsg=\"error parements\"}\n";
            }
        } catch (const exception& e) {
            ostr << "{retcode:-1,retmsg=\"" << e.what() << "\"}\n";
        } catch (...) {
            ostr << "{retcode:-1,retmsg=\"unknow exception.\"}\n";
        }
    } else if (uri.find("/Api/eds/tgn/") != std::string::npos) {
        try {
            std::vector<std::string> wizids;
            tidp::TL_Common::split(wizids, uri.substr(sizeof ("/Api/eds/tgn/") - 1), ",");
            //ostr << "wizids.size = " << wizids.size() << "<br/>\n";
            if (wizids.size() > 0) {
                ostr << "{retcode:0,data:[";
                std::map<int, myPointValue> values;
                std::vector<std::string>::const_iterator it = wizids.begin();
                while (it != wizids.end()) {
                    map<string, pair<EdsPointThread*, int> >::const_iterator it2 = _tgn_thread_liveid.find(*it);
                    if (it2 != _tgn_thread_liveid.end()) {
                        ostr << "{tgn:\"" << *it << "\",data:[";
                        if (it2->second.first->getPointValuesFromCache(it2->second.second, values)) {
                            if (values.size() > 0) {
                                std::map<int, myPointValue>::const_reverse_iterator it3 = values.rbegin();
                                if (it3 != values.rend()) {
                                    const myPointValue& mpv = it3->second;
                                    ostr << "{ts:" << mpv.ts << ",type:'" << mpv.type << "',quality:'" << mpv.quality << "',value:";
                                    if (mpv.type != 'B' && mpv.type != 'P') {
                                        ostr << mpv.value.f;
                                    } else {
                                        ostr << mpv.value.i;
                                    }
                                    ostr << "}";
                                }
                                /*
                                while (it3 != values.rend()) {
                                    const myPointValue& mpv = it3->second;
                                    ostr << "{ts:" << mpv.ts << ",type:'" << mpv.type << "',quality:'" << mpv.quality << "',value:";
                                    if (mpv.type != 'B' && mpv.type != 'P') {
                                        ostr << mpv.value.f;
                                    } else {
                                        ostr << mpv.value.i;
                                    }
                                    ostr << "}";
                                    if (++i > 5) break;
                                    ++it3;
                                    if (it3 != values.rend()) {
                                        ostr << ",";
                                    }
                                }*/
                            }
                        }
                        ostr << "]}";
                    } else {
                        ++it;
                        continue;
                    }
                    ++it;
                    if (it != wizids.end()) ostr << ",";
                }
            }
            ostr << "]}";
        } catch (const exception& e) {
            ostr << "{retcode:-1,retmsg=\"" << e.what() << "\"}\n";
        } catch (...) {
            ostr << "{retcode:-1,retmsg=\"unknow exception.\"}\n";
        }
    } else if (uri.find("/Api/eds/status") != std::string::npos) {
        vector<EdsPointThread*>::iterator it = _eds_threads.begin();
        ostr << "[";
        while (it != _eds_threads.end()) {
            ostr << (*it)->status2Json();
            ++it;
            if (it != _eds_threads.end()) {
                ostr << ",\n";
            }
        }
        ostr << "]";
    } else if (uri.find("/Api/eds/zero/") != std::string::npos) {
        string numstr = uri.substr(sizeof ("/Api/eds/zero/") - 1);
        int tno = -1;
        if (numstr.size() > 0) {
            tno = atoi(numstr.c_str());
            vector<EdsPointThread*>::iterator it = _eds_threads.begin();
            while (it != _eds_threads.end()) {
                EdsPointThread* e = *it;
                if (e->getThreadNo() == tno) {
                    e->zeroTsIds2Json(ostr);
                    return;
                }
                ++it;
            }
        }
        ostr << "{retcode:-1,retmsg=\"unknow thread id:" << tno << "|" << numstr << "\"}";
    } else if (uri.find("/Api/eds/history/") != std::string::npos) {
        string subquery = uri.substr(sizeof ("/Api/eds/history/") - 1);
        //tgn/$tgn/$stime/$etime
        //tgn配置文件的迁移,会对该方案产生影响
        std::vector<std::string> tmp;
        tidp::TL_Common::split(tmp, subquery, "/");
        if (tmp.size() < 2) {
            ostr << "{retcode:-1,retmsg=\"param not enough\"}";
        } else {
            time_t now = time(NULL);
            string tgn = tmp[0];
            time_t stime = 0;
            if (tmp[1].size() > 0) stime = atoll(tmp[1].c_str());
            time_t etime = 0;
            if (tmp.size() > 2 && tmp[2].size() > 0) etime = atoll(tmp[2].c_str());
            if (etime <= 0 || etime > now) etime = now;
            if (tgn.size() < 3 || stime > etime || stime <= 0 || etime <= 0) {
                ostr << "{retcode:-1,retmsg=\"param  error:[" << tgn << "," << stime << "," << etime << "]\"}";
            } else if (etime - stime > DAY_SECONDS ||
                    (stime + BEIJING_TIME_INV) / DAY_SECONDS != (etime + BEIJING_TIME_INV) / DAY_SECONDS
                    ) {
                ostr << "{retcode:-1,retmsg=\"stime etime not in same day.\"}";
            } else {
                map<string, pair<EdsPointThread*, int> >::const_iterator it = _tgn_thread_liveid.find(tgn);
                if (it == _tgn_thread_liveid.end()) {
                    ostr << "{retcode:-1,retmsg=\"tgn:" << tgn << " is not available.\"}";
                } else {
                    EdsPointThread* eptr = it->second.first;
                    eptr->history(ostr, tgn, stime, etime);
                }
            }
        }
        
    } else {
        ostr << "{retcode:-1,retmsg=\"unknow api.\"}\n";
    }
    ///Api/eds/wizids/PR.AUX.AI.Y0BHE02CE101,PR.AUX.AI.Y0GCB12DP101
}

EdsHttpServer::EdsHttpServer() {
}

EdsHttpServer::EdsHttpServer(const EdsHttpServer& orig) {
}

EdsHttpServer::~EdsHttpServer() {
}

void EdsHttpServer::initialize(Application& self) {
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
}

void EdsHttpServer::uninitialize() {
    ServerApplication::uninitialize();
}

int EdsHttpServer::main(const std::vector<std::string>& arg) {
    ServerSocket svs(9191);
    HTTPServer svr(new EdsRequestHandlerFactory(), svs, new HTTPServerParams);
    cout << "svr.start ..." << endl;
    svr.start();
    waitForTerminationRequest();
    cout << "stop ..." << endl;
    svr.stop();
    return Application::EXIT_OK;
}
