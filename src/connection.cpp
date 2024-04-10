#include "Connection.h"


MySQL::~MySQL() {
    if(_con != nullptr)
        _con->close();
}

bool MySQL::connect() {
    sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
    std::string con_str = _ip + ":" + std::to_string(_port);
    try {
        _con.reset(driver->connect(con_str, _user, _password));
        if(_con != nullptr) {
            _stmt.reset(_con->createStatement());
            _stmt->execute("use " + _dbName);
            return true;
        }
        return false;
    }
    catch(const sql::SQLException& e)
    {
        // 处理异常，这里返回false表示连接失败  
        error_process(e);
        return false;  
    }
    
}

bool MySQL::update(const std::string &sql) {
    try {
        _stmt->execute(sql);
        return true;
    } catch(const sql::SQLException &e) {
        error_process(e);
        return false;
    }

}


sql::ResultSet* MySQL::query(const std::string &sql) {
    try
    {
        sql::ResultSet* res = _stmt->executeQuery(sql);
        return res;
    }
    catch(const sql::SQLException& e)
    {
        // LOG("查询失败： " + sql);
        error_process(e);
        return nullptr;
    }
    
}

void MySQL::refreshAliveTime() {
    _aliveTime = clock();
}

clock_t MySQL::getAliveTime() {
    return clock() - _aliveTime;
}

void MySQL::error_process(const sql::SQLException& e) {
    std::cerr << "# ERR: SQLException in " << __FILE__;  
    std::cerr << " on line " << __LINE__ << std::endl;  
    std::cerr << "# ERR: " << e.what();  
    std::cerr << " (MySQL error code: " << e.getErrorCode();  
    std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;  
}