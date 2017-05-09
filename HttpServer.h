/* 
 * File:   HttpServer.h
 * Author: ever
 *
 * Created on 2016年4月29日, 上午9:30
 */

#ifndef HTTPSERVER_H
#define	HTTPSERVER_H
#include <iostream>
#include <string>
#include <vector>

#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Path.h>
#include <Poco/URI.h>

#include "TL_Common.h"
using namespace Poco::Net;
using namespace Poco::Util;

class EdsRequestHandler : public HTTPRequestHandler{
public:
    EdsRequestHandler(){}
    void handleRequest(HTTPServerRequest& request,HTTPServerResponse& response);
};
class EdsRequestHandlerFactory : public HTTPRequestHandlerFactory{
public:
    EdsRequestHandlerFactory(){}
    HTTPRequestHandler * createRequestHandler(const HTTPServerRequest& request){
        //cout << "EdsRequestHandlerFactor " << request.getURI() << endl;
        return new EdsRequestHandler;
    }
};


class EdsHttpServer : public ServerApplication{
public:
    EdsHttpServer();
    EdsHttpServer(const EdsHttpServer& orig);
    virtual ~EdsHttpServer();
protected:
    void initialize(Application& self);
    void uninitialize();
    
    
    int main(const std::vector<std::string>& arg);
private:
};

#endif	/* HTTPSERVER_H */

