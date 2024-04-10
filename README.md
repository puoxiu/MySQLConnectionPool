

# 基于c++11的数据库连接池

## 关键技术：

MySQL数据库编程

设计模式：单例、queue队列容器

C++11：多线程编程（线程互斥、线程同步、线程通信）、原子整形、智能指针、lambada

池化设计：效率提上、资源节省、便于管理



## 背景：

目的：

1. 提高MySQL访问瓶颈的一种方式是缓存数据库的使用（例如Redis）
2. 另一种方式是增加连接池--通过减少频繁创建和关闭数据库连接的开销来优化资源使用。

多线程模型：生产者-消费者模型

## 介绍：

### 单例模式--懒汉实现

单例模式确保一个类只有一个实例，并提供一个全局访问点来获取这个唯一实例.

实现通常涉及到一个静态的私有成员变量来保存单例对象，以及一个公有的静态方法（惯例命名为`getInstance`）来返回这个单例对象

实现有：

1. 懒汉模式：单例对象直到第一次被请求时才被创建，这意味着单例对象的实例化是延迟的，以节省资源
2. 饿汉模式，单例对象在程序启动时就被创建，无论是否被使用

这里采用了懒汉模式:  即在刚开始即实例化对象。但是懒汉模式在多线程环境下要注意**线程安全**，防止在多线程中创建出多个实例。


   本系统采用了双重检查锁定方法：

   ```c++
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
   ```

### 智能指针--删除器

智能指针析构时默认的删除器会释放资源，需指定删除器: 析构时将从连接队列获取的资源归还队列而不是释放

```c++
    shared_ptr<MySQL> sp(_connecionQueue.front(), [&](MySQL *m){
        unique_lock<mutex> lock(_queueMutex);
        m->refreshAliveTime();
        _connecionQueue.push(m);
    });
```







## 快速运行

### 环境

* 连接MySQL云服务器：8.138.96.253（2核、4G）
* 编译环境：deepin 版本：20.9， Linux内核版本：5.15.77
* 数据库配置文件：public.h


编译运行：

进入build目录：

```shell
cmake ..
make
./dbPool.h
```



## 压力测试



| 数据 |             不使用连接池             | 使用连接池 |
| :--: | :----------------------------------: | :--------: |
| 1200 |  单线程：80166ms    双线程：48499ms  |  10910 ms  |
| 8000 | 单线程：503834ms    双线程：351931ms |  85187 ms  |


* 是一个学习项目，还没有达到可以在生产环境中使用的能力