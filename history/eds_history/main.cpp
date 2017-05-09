/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: ever
 *
 * Created on 2017年4月11日, 下午2:47
 */

#include <cstdlib>
#include "HttpServer.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    HttpServer http;
    http.run(1, argv);
    return Application::EXIT_OK;
}

