#ifndef STUDENT_DB_H
#define STUDENT_DB_H

#include "MySQLConnPool.h"
#include <string>
#include <memory> // ��Ҫ shared_ptr

class StudentDB {
public:
    // ��ȡ����ʵ��
    static StudentDB& getInstance();

    // ѧ��ע�ᣬ���� true ��ʾע��ɹ���false ��ʾʧ�ܣ���ѧ���Ѵ��ڣ�
    bool registerStudent(const std::string& studentId, const std::string& password);

    // ѧ����¼��֤������ true ��ʾ�ɹ���false ��ʾʧ��
    bool verifyStudent(const std::string& studentId, const std::string& password);

    // ��ȡѧ���ĸ���ǩ����ʧ��ʱ���ؿ��ַ���
    std::string getSignature(const std::string& studentId);

    // ����ѧ���ĸ���ǩ�������� true ��ʾ���³ɹ���false ��ʾʧ��
    bool updateSignature(const std::string& studentId, const std::string& newSignature);

private:
    // ���캯��˽�л���ȷ������ģʽ
    StudentDB();
    StudentDB(const StudentDB&) = delete;
    StudentDB& operator=(const StudentDB&) = delete;

    MySQLConnPool* connPool; // ���ݿ����ӳ�
};

#endif // STUDENT_DB_H
