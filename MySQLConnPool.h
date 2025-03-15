#ifndef MYSQLCONNPOOL_H
#define MYSQLCONNPOOL_H

#include <mysql/mysql.h>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

class MySQLConn {
public:
    MySQLConn();
    ~MySQLConn();
    bool connect(const std::string& host, const std::string& user, const std::string& password,
        const std::string& dbname, unsigned int port);
    MYSQL* getConnection();
    void close();
private:
    MYSQL* conn;
};

// 数据库连接池
class MySQLConnPool {
public:
    static MySQLConnPool* getInstance();
    void init(const std::string& host, const std::string& user, const std::string& password,
        const std::string& dbname, unsigned int port, int maxSize);
    std::shared_ptr<MySQLConn> getConnection();
    void releaseConnection(MySQLConn* conn);
    ~MySQLConnPool();
private:
    MySQLConnPool();
    std::queue<MySQLConn*> pool;
    std::mutex mtx;
    std::condition_variable cond;
    int maxSize;
    int curSize;
    std::string dbHost, dbUser, dbPass, dbName;
    unsigned int dbPort;
};

#endif // MYSQLCONNPOOL_H
