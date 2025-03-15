#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>
#include <unordered_map>
#include <sstream>

// HTTP ����ṹ��
struct HttpRequest {
    std::string method;                        // ���󷽷� (GET / POST)
    std::string path;                          // ����·��
    std::unordered_map<std::string, std::string> headers;  // ����ͷ��
    std::string body;                          // ������
    std::string query_string;
};

// �н���״̬��
enum LineState { LINE_OK, LINE_BAD, LINE_OPEN };

// HTTP ����״̬��
enum ParseState { PARSE_REQUEST_LINE, PARSE_HEADERS, PARSE_BODY, PARSE_FINISHED };

// ����һ�У��ж��Ƿ�����
LineState getLine(const char* buffer, int& pos, std::string& line);

// ����������
bool parseRequestLine(const std::string& line, HttpRequest& request);

// ���� HTTP ����
bool parseHttpRequest(const char* buffer, HttpRequest& request);

#endif // HTTP_PARSER_H
