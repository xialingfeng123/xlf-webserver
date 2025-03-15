#include "MySQLConnPool.h"
#include <iostream>

// MySQLConn ���캯��
MySQLConn::MySQLConn() {
    conn = mysql_init(nullptr);
}

// �������ݿ�
bool MySQLConn::connect(const std::string& host, const std::string& user,
    const std::string& password, const std::string& dbname, unsigned int port) {
    return mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
}

// ��ȡԭʼ MYSQL ����
MYSQL* MySQLConn::getConnection() {
    return conn;
}

// �ر�����
void MySQLConn::close() {
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
    }
}

// MySQLConn ��������
MySQLConn::~MySQLConn() {
    close();
}

// ========== MySQLConnPool ʵ�� ==========

// ����ģʽ
MySQLConnPool* MySQLConnPool::getInstance() {
    static MySQLConnPool instance;
    return &instance;
}

// ���ӳع��캯��
MySQLConnPool::MySQLConnPool() : maxSize(10), curSize(0) {}

// ��ʼ�����ӳ�
void MySQLConnPool::init(const std::string& host, const std::string& user, const std::string& password,
    const std::string& dbname, unsigned int port, int maxSize) {
    this->dbHost = host;
    this->dbUser = user;
    this->dbPass = password;
    this->dbName = dbname;
    this->dbPort = port;
    this->maxSize = maxSize;

    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < maxSize / 2; ++i) { // �ȴ���һ������
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

// ��ȡ����
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

// �黹����
void MySQLConnPool::releaseConnection(MySQLConn* conn) {
    std::lock_guard<std::mutex> lock(mtx);
    pool.push(conn);
    cond.notify_one();
}

// ���ӳ���������
MySQLConnPool::~MySQLConnPool() {
    std::lock_guard<std::mutex> lock(mtx);
    while (!pool.empty()) {
        delete pool.front();
        pool.pop();
    }
}
