/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpServer.cpp
 * Author: ever
 * 
 * Created on 2017年4月11日, 下午2:49
 */

#include "HttpServer.h"

void EdsRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    ///history/tgn/$tgn/stime/$stime/etime/$etime
    std::ostream & ostr = response.send();
    const std::string& uri = request.getURI();
    if (uri.find("/Api/eds/history/tgn/") != std::string::npos) {
        std::vector<std::string> qstr;
        tidp::TL_Common::split(qstr, uri.substr(sizeof ("/Api/eds/history/") - 1), "/");
        if (qstr.size() < 6) {
            ostr << "{retcode:-1,retmsg=\"param error.\"}\n";
        } else {
            time_t stime = 0;
            time_t etime = 0;
            string tgn;
            vector<std::string>::iterator it = qstr.begin();
            while (it != qstr.end()) {
                if (*it == "tgn") {
                    ++it;
                    tgn = *it;
                } else if (*it == "stime") {
                    ++it;
                    stime = strtoll((*it).c_str(), NULL, 10);
                } else if (*it == "etime") {
                    ++it;
                    etime = strtoll((*it).c_str(), NULL, 10);
                } else {
                    ++it;
                }
            }
            if (tgn.empty()) {
                ostr << "{retcode:-1,retmsg=\"tgn is empty.\"}\n";
            } else {
                string fname = tgn;
                FILE * fp = fopen(fname.c_str(), "r");
                //每次读取一个文件
                if (fp == NULL) {
                    ostr << "{retcode:-1,retmsg=\"can't open " << fname << "\"}\n";
                } else {
                    char *buffer = (char *) malloc(2048);
                    fread(buffer, 1, 1024, fp);
                    compressBlock * block = (compressBlock *) buffer;

                    free(buffer);
                }
            }
        }
    } else {
        ostr << "{retcode:-1,retmsg=\"unknow api.\"}\n";
    }
}

HttpServer::HttpServer() {
}

HttpServer::HttpServer(const HttpServer& orig) {
}

HttpServer::~HttpServer() {

}

