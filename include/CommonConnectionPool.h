#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <memory>
#include "Connection.h"


class ConnectionPool {
public:
    static ConnectionPool* getInstance();

    /*（消费者线程）
        给某个需求取出一个连接，用智能指针且重新设定删除器：在资源释放时不做析构而
    */
    std::shared_ptr<MySQL> getConnection();
private:
    ConnectionPool();
    ~ConnectionPool();

    static ConnectionPool* _instance;
    static std::mutex _mutex;

    // 运行在生产者线程中
    void produceConnectionTask();
    
    /*
        线程函数：扫描超出_maxIdleTime的空闲连接
        扫描队列，当前队列的连接数量大于初始值，且队头连接（存活时间最久）的存活时间大于_maxIdleTime时，释放队头连接
    */ 
    void scannerConnectionTask();


    std::string _ip;
    unsigned int _port;
    std::string _username;
    std::string _password;
    std::string _dbName;

    std::atomic_int _connectionCount;    // 当前连接数量
    int _initSize;      // 连接池初始连接量
    int _maxSize;       // 最大连接量
    int _maxIdleTime;   // 最大空闲时间
    int _maxConnectionTimeOut;           // 获取连接池连接的超时时间
    std::queue<MySQL*> _connecionQueue;  // 存储所有连接的队列
    std::mutex _queueMutex;              // 互斥锁维护列列
    std::condition_variable conv;        // 条件变量-线程同步
};