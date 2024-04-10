#include "CommonConnectionPool.h"
#include "public.h"

using std::shared_ptr;
using std::mutex;
using std::unique_lock;
using std::thread;

ConnectionPool* ConnectionPool::_instance = nullptr;
std::mutex ConnectionPool::_mutex;

ConnectionPool* ConnectionPool::getInstance() {
    if(!_instance) {
        std::lock_guard<mutex> lock(_mutex);
        if(!_instance) {
            _instance = new ConnectionPool();
        }
    }
    return _instance;
}

ConnectionPool::ConnectionPool() {
    _ip = IP;
    _port = PORT;
    _username = USER;
    _password = PASSWORD;
    _dbName = DBNAME;
    
    _initSize = INITSIZE;
    _connectionCount = 0;
    _maxSize = MAXSIZE;

    _maxIdleTime = MAXIDLETIME;
    _maxConnectionTimeOut = MAXCONNECTIONTIMEOUT;

    // 创建初始连接
    for(int i = 0; i < _initSize; i++) {
        MySQL *m = new MySQL(_ip, _port, _username, _password, _dbName);
        if(m->connect()) {
            m->refreshAliveTime();
            _connecionQueue.push(m);
            _connectionCount++;
        }
    }
    // 生产者线程
    thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();
    //
    thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

void ConnectionPool::produceConnectionTask() {
    while(1) {
        std::unique_lock<mutex> lock(_queueMutex);
        while(!_connecionQueue.empty()) {
            conv.wait(lock);        // 连接池队列中的连接还没用完，在此等待消费者线程的唤醒
        }
        if(_connectionCount < _maxSize) {
            MySQL *m = new MySQL(_ip, _port, _username, _password, _dbName);
            if(m->connect()) {
                _connecionQueue.push(m);
                _connectionCount++;
            }
        }
        // 通知所有消费者线程
        conv.notify_all();
    }
}

void ConnectionPool::scannerConnectionTask() {
    while(1) {
        // 通过sleep模拟定时效果
        std::this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));
        //
        unique_lock<mutex> lock(_queueMutex);
        while(_connectionCount > _initSize) {
            MySQL* m = _connecionQueue.front();
            if(m->getAliveTime() >= _maxIdleTime * 1000) {
                _connecionQueue.pop();
                _connectionCount--;
                delete m;
            } else {
                break;
            }
        }
    }
}


shared_ptr<MySQL> ConnectionPool::getConnection() {
    unique_lock<mutex> lock(_queueMutex);
    while(_connecionQueue.empty()) {
        if (std::cv_status::timeout == conv.wait_for(lock, std::chrono::milliseconds(_maxConnectionTimeOut))) {
            if(_connecionQueue.empty()) {
                LOG("失败：获取空闲连接超时");
                return nullptr;
            }
        }
    }
    /*
        智能指针析构时默认会释放资源，需指定删除器: 析构时将资源归还的队列而不是释放
    */
    shared_ptr<MySQL> sp(_connecionQueue.front(), [&](MySQL *m){
        unique_lock<mutex> lock(_queueMutex);
        m->refreshAliveTime();
        _connecionQueue.push(m);
    });
    _connecionQueue.pop();
    
    conv.notify_all(); 

    return sp;
}


ConnectionPool::~ConnectionPool() {

}