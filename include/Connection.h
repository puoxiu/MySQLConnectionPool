#pragma once
#include <mysql_driver.h>  
#include <mysql_connection.h>  
#include <cppconn/driver.h>  
#include <cppconn/exception.h>  
#include <cppconn/resultset.h>  
#include <cppconn/statement.h>  
#include <string>
#include <ctime>
#include <memory>

#include "public.h"


class MySQL {
public:
    MySQL(const std::string ip, const unsigned int port, const std::string user, const std::string password, 
            const std::string dbName): _ip(ip), _port(port), _user(user), _password(password), _dbName(dbName)
            {
            }
    ~MySQL();

    bool connect();
    bool update(const std::string &sql);        // 更新操作包括：delete insert update
    sql::ResultSet* query(const std::string &sql);     // 查询操作:query

    void refreshAliveTime();        // 刷新进入连接池队列的时刻
    clock_t getAliveTime();         // 得到连接存活时间
    void error_process(const sql::SQLException& e);

private:
    clock_t _aliveTime;     // 进入队列后的时刻

    std::unique_ptr<sql::Connection> _con;
    std::unique_ptr<sql::Statement> _stmt;
    std::string _ip;  
    unsigned int _port;
    std::string _user;  
    std::string _password;  
    std::string _dbName; 
};