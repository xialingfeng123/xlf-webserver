#include "WebServer.h"
#include "MySQLConnPool.h"
#include "StudentDB.h"


int main() {
    // ��ʼ�����ݿ����ӳ�
    MySQLConnPool* dbPool = MySQLConnPool::getInstance();
    dbPool->init("localhost", "webuser", "mypassword", "student_db", 3306, 10);

    // ���� Web ������
    WebServer server(SERVER_PORT, THREAD_POOL_SIZE);
    server.run();

    return 0;
}
