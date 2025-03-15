#include "StudentDB.h"
#include <iostream>
#include <sstream>

StudentDB& StudentDB::getInstance() {
    static StudentDB instance;
    return instance;
}

// ���캯����˽�л���
StudentDB::StudentDB() {
    connPool = MySQLConnPool::getInstance();
}

// ѧ��ע��
bool StudentDB::registerStudent(const std::string& studentId, const std::string& password) {
    std::shared_ptr<MySQLConn> conn = connPool->getConnection();
    if (!conn) return false;

    std::ostringstream query;
    query << "INSERT INTO students (student_id, password) VALUES ('"
        << studentId << "', '" << password << "')";

    if (mysql_query(conn->getConnection(), query.str().c_str()) == 0) {
        return true;
    }
    else {
        std::cerr << "[ERROR] Failed to register student: " << mysql_error(conn->getConnection()) << std::endl;
        return false;
    }
}

// ѧ����¼
bool StudentDB::verifyStudent(const std::string& studentId, const std::string& password) {
    std::shared_ptr<MySQLConn> conn = connPool->getConnection();
    if (!conn) return false;

    std::ostringstream query;
    query << "SELECT * FROM students WHERE student_id='"
        << studentId << "' AND password='" << password << "'";

    if (mysql_query(conn->getConnection(), query.str().c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn->getConnection());
        bool success = (mysql_num_rows(res) > 0);
        mysql_free_result(res);
        return success;
    }
    else {
        std::cerr << "[ERROR] Failed to verify student: " << mysql_error(conn->getConnection()) << std::endl;
        return false;
    }
}

// ��ȡ����ǩ��
std::string StudentDB::getSignature(const std::string& studentId) {
    std::shared_ptr<MySQLConn> conn = connPool->getConnection();
    if (!conn) return "";

    std::ostringstream query;
    query << "SELECT signature FROM students WHERE student_id='" << studentId << "'";

    if (mysql_query(conn->getConnection(), query.str().c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn->getConnection());
        MYSQL_ROW row = mysql_fetch_row(res);
        std::string signature = (row ? row[0] : "");
        mysql_free_result(res);
        return signature;
    }
    else {
        std::cerr << "[ERROR] Failed to get signature: " << mysql_error(conn->getConnection()) << std::endl;
        return "";
    }
}

// ���¸���ǩ��
bool StudentDB::updateSignature(const std::string& studentId, const std::string& newSignature) {
    std::shared_ptr<MySQLConn> conn = connPool->getConnection();
    if (!conn) return false;

    std::ostringstream query;
    query << "UPDATE students SET signature='" << newSignature
        << "' WHERE student_id='" << studentId << "'";

    if (mysql_query(conn->getConnection(), query.str().c_str()) == 0) {
        return (mysql_affected_rows(conn->getConnection()) > 0);
    }
    else {
        std::cerr << "[ERROR] Failed to update signature: " << mysql_error(conn->getConnection()) << std::endl;
        return false;
    }
}
