#pragma once
#include <iostream>


// 日志的封装
#define LOG(str) \
    std::cout << __FILE__ << ":" << __LINE__ << " " << \
    __TIMESTAMP__ << " : " << str << std::endl;


    #define IP "8.138.96.253"
    #define PORT 3306
    #define USER "xing"
    #define PASSWORD "990808"
    #define DBNAME "mybooks"

    #define INITSIZE 10
    #define MAXSIZE 1024
    // s
    #define MAXIDLETIME 60
    //  ms
    #define MAXCONNECTIONTIMEOUT 100
