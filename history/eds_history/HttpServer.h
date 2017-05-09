/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpServer.h
 * Author: ever
 *
 * Created on 2017年4月11日, 下午2:49
 */

#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <time.h>
#include <stdlib.h>

#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Path.h>
#include <Poco/URI.h>

#include "HttpServer.h"
#include "TL_Common.h"


using namespace Poco::Net;
using namespace Poco::Util;
using namespace std;


struct compressBlock{
    int block_id;
    int block_start_off;
    int block_size;
    int block_elm_count;
};


class EdsRequestHandler : public HTTPRequestHandler {
public:

    EdsRequestHandler() {
    }
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);
};

class EdsRequestHandlerFactor : public HTTPRequestHandlerFactory {
public:

    EdsRequestHandlerFactor() {
    }

    HTTPRequestHandler * createRequestHandler(const HTTPServerRequest& request) {
        return new EdsRequestHandler;
    }
};

class HttpServer : public ServerApplication {
public:
    HttpServer();
    HttpServer(const HttpServer& orig);
    virtual ~HttpServer();
private:

};

#endif /* HTTPSERVER_H */

