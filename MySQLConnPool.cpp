#include "MySQLConnPool.h"
#include <iostream>

// MySQLConn 构造函数
MySQLConn::MySQLConn() {
    conn = mysql_init(nullptr);
}

// 连接数据库
bool MySQLConn::connect(const std::string& host, const std::string& user,
    const std::string& password, const std::string& dbname, unsigned int port) {
    return mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
}

// 获取原始 MYSQL 连接
MYSQL* MySQLConn::getConnection() {
    return conn;
}

// 关闭连接
void MySQLConn::close() {
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
    }
}

// MySQLConn 析构函数
MySQLConn::~MySQLConn() {
    close();
}

// ========== MySQLConnPool 实现 ==========

// 单例模式
MySQLConnPool* MySQLConnPool::getInstance() {
    static MySQLConnPool instance;
    return &instance;
}

// 连接池构造函数
MySQLConnPool::MySQLConnPool() : maxSize(10), curSize(0) {}

// 初始化连接池
void MySQLConnPool::init(const std::string& host, const std::string& user, const std::string& password,
    const std::string& dbname, unsigned int port, int maxSize) {
    this->dbHost = host;
    this->dbUser = user;
    this->dbPass = password;
    this->dbName = dbname;
    this->dbPort = port;
    this->maxSize = maxSize;

    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < maxSize / 2; ++i) { // 先创建一半连接
        MySQLConn* conn = new MySQLConn();
        if (conn->connect(dbHost, dbUser, dbPass, dbName, dbPort)) {
            pool.push(conn);
            curSize++;
        }
        else {
            delete conn;
        }
    }
}

// 获取连接
std::shared_ptr<MySQLConn> MySQLConnPool::getConnection() {
    std::unique_lock<std::mutex> lock(mtx);
    while (pool.empty()) {
        cond.wait(lock);
    }

    MySQLConn* conn = pool.front();
    pool.pop();

    return std::shared_ptr<MySQLConn>(conn, [&](MySQLConn* c) {
        this->releaseConnection(c);
        });
}

// 归还连接
void MySQLConnPool::releaseConnection(MySQLConn* conn) {
    std::lock_guard<std::mutex> lock(mtx);
    pool.push(conn);
    cond.notify_one();
}

// 连接池析构函数
MySQLConnPool::~MySQLConnPool() {
    std::lock_guard<std::mutex> lock(mtx);
    while (!pool.empty()) {
        delete pool.front();
        pool.pop();
    }
}
