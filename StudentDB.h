#ifndef STUDENT_DB_H
#define STUDENT_DB_H

#include "MySQLConnPool.h"
#include <string>
#include <memory> // 需要 shared_ptr

class StudentDB {
public:
    // 获取单例实例
    static StudentDB& getInstance();

    // 学生注册，返回 true 表示注册成功，false 表示失败（如学号已存在）
    bool registerStudent(const std::string& studentId, const std::string& password);

    // 学生登录验证，返回 true 表示成功，false 表示失败
    bool verifyStudent(const std::string& studentId, const std::string& password);

    // 获取学生的个性签名，失败时返回空字符串
    std::string getSignature(const std::string& studentId);

    // 更新学生的个性签名，返回 true 表示更新成功，false 表示失败
    bool updateSignature(const std::string& studentId, const std::string& newSignature);

private:
    // 构造函数私有化，确保单例模式
    StudentDB();
    StudentDB(const StudentDB&) = delete;
    StudentDB& operator=(const StudentDB&) = delete;

    MySQLConnPool* connPool; // 数据库连接池
};

#endif // STUDENT_DB_H
