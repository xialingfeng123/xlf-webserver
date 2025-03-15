#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>
#include <unordered_map>
#include <sstream>

// HTTP 请求结构体
struct HttpRequest {
    std::string method;                        // 请求方法 (GET / POST)
    std::string path;                          // 访问路径
    std::unordered_map<std::string, std::string> headers;  // 请求头部
    std::string body;                          // 请求体
    std::string query_string;
};

// 行解析状态机
enum LineState { LINE_OK, LINE_BAD, LINE_OPEN };

// HTTP 解析状态机
enum ParseState { PARSE_REQUEST_LINE, PARSE_HEADERS, PARSE_BODY, PARSE_FINISHED };

// 解析一行，判断是否完整
LineState getLine(const char* buffer, int& pos, std::string& line);

// 解析请求行
bool parseRequestLine(const std::string& line, HttpRequest& request);

// 解析 HTTP 请求
bool parseHttpRequest(const char* buffer, HttpRequest& request);

#endif // HTTP_PARSER_H
