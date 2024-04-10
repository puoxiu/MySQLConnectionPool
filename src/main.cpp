#include "Connection.h"
#include "CommonConnectionPool.h"
#include <ctime>
#include <iostream>
#include <vector>

/*
    压力测试：2000、8000次的insert数据
    测试使用连接池(单线程、多线程）：
    和不使用连接池所用时间
*/ 


// 不使用连接池单线程
void testMySQL()
{

    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 8000; i++) {
        MySQL M("8.138.96.253", 3306, "xing", "990808", "mybooks");
        if(!M.connect()) {
            LOG("连接数据库失败！");
            return;
        }
        std::string insert_sql = "insert into users(username, password) values ('ming";
        insert_sql += std::to_string(i) + "', '990808')";

        // std::cout << insert_sql << std::endl;
        
        if(!M.update(insert_sql)) {
            LOG("插入失败");
        } else {
            if(i % 100 == 0) {
                std::cout << "已插入：" << i << std::endl;
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();  
    std::cout << "占用时间： " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
}

void testMySQLPool() {
    auto pool = ConnectionPool::getInstance();

    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < 8000; i++) {
        std::string insert_sql = "insert into users(username, password) values ('ming";
        insert_sql += std::to_string(i) + "', '990808')";

        auto pc = pool->getConnection();
        if(! pc->update(insert_sql)) {
            LOG("插入失败");
        } else {
            if(i % 100 == 0) {
                std::cout << "已插入：" << i << std::endl;
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();  
    std::cout << "占用时间： " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl; 
}


void func(int start, int num) {
    //  std::cout << "insert_sql" << std::endl;
        for(int i = start; i < start+ num; i++) {
            MySQL M("8.138.96.253", 3306, "xing", "990808", "mybooks");
            if(!M.connect()) {
                LOG("连接数据库失败！");
                return;
            }
            std::string insert_sql = "insert into users(username, password) values ('ming";
            insert_sql += std::to_string(i) + "', '990808')";

            // std::cout << insert_sql << std::endl;
            
            if(!M.update(insert_sql)) {
                LOG("插入失败");
            }
        }
}
void testMySQLThreads() {

    std::vector<std::thread> threads; 
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 2; ++i) {  
        threads.emplace_back([=](){
            for(int i = 0; i < 4000; i++) {
                MySQL M("8.138.96.253", 3306, "xing", "990808", "mybooks");
                if(!M.connect()) {
                    LOG("连接数据库失败！");
                    return;
                }
                std::string insert_sql = "insert into users(username, password) values ('ming";
                insert_sql += std::to_string(i * 4000) + "', '990808')";

                // std::cout << insert_sql << std::endl;
                
                if(!M.update(insert_sql)) {
                    LOG("插入失败");
                }
            }
        });
    }  
    for (auto& t : threads) {  
        t.join();  
    }  
    auto end = std::chrono::high_resolution_clock::now();  
    std::cout << "占用时间： " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl; 
}
int main()
{
    // testMySQL();

    testMySQLPool();

    // testMySQLThreads();


        // auto res = M.query("select * from users;");
        // while(res && res->next()) {
            
        //     std::cout << res->getString("username") << " " << res->getString("password") << std::endl;
        // }
    return 0;
}



