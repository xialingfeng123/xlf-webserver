#include "HttpParser.h"
#include <sstream>
#include <algorithm> 
#include <iostream>
#include <cstring>


LineState getLine(const char* buffer, int& pos, std::string& line) {
    line.clear();
    while (buffer[pos] != '\0') {
        if (buffer[pos] == '\r' && buffer[pos + 1] == '\n') {
            pos += 2;
            return LINE_OK;
        }
        line += buffer[pos++];
    }
    return LINE_OPEN;
}


bool parseRequestLine(const std::string& line, HttpRequest& request) {
    std::istringstream stream(line);
    stream >> request.method >> request.path;
    std::string version;
    stream >> version;
    return !(request.method.empty() || request.path.empty() || version.empty());
}


#include "HttpParser.h"
#include <sstream>
#include <iostream>


bool parseHttpRequest(const char* buffer, HttpRequest& request) {
    std::istringstream stream(buffer);
    std::string line;
    int content_length = 0;

    
    if (!std::getline(stream, line) || line.empty()) return false;
    std::istringstream request_line(line);
    request_line >> request.method >> request.path;

   
    size_t pos = request.path.find("?");
    if (pos != std::string::npos) {
        request.query_string = request.path.substr(pos + 1);
        request.path = request.path.substr(0, pos);
    }

    std::string version;
    request_line >> version;
    if (version != "HTTP/1.1") return false; 

   
    while (std::getline(stream, line) && !line.empty()) {
        if (line == "\r") break; 
        size_t colon = line.find(":");
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 2); 
            request.headers[key] = value;
            if (key == "Content-Length") {
                content_length = std::stoi(value);
            }
        }
    }


    if (request.method == "POST" && content_length > 0) {
        request.body.resize(content_length);  
        stream.read(&request.body[0], content_length);  
        //std::cout<<"body:"<<request.body<<std::endl;
    }
    return true;
}


