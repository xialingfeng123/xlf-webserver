#include "WebServer.h"
#include "MySQLConnPool.h"
#include "StudentDB.h"


int main() {
    // 初始化数据库连接池
    MySQLConnPool* dbPool = MySQLConnPool::getInstance();
    dbPool->init("localhost", "webuser", "mypassword", "student_db", 3306, 10);

    // 启动 Web 服务器
    WebServer server(SERVER_PORT, THREAD_POOL_SIZE);
    server.run();

    return 0;
}
