#include "WebServer.h"
#include "HttpParser.h"
#include "StudentDB.h"
#include <sstream>
#include <fstream>  
#include <iostream>
#include <unordered_map>
#include <cstring>

WebServer::WebServer(int port, int num_threads)
    : thread_pool(num_threads), running(true), db(StudentDB::getInstance()) {
    setupServer(port);
}

WebServer::~WebServer() {
    stop();
}

void WebServer::setupServer(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1024);

    epoll_fd = epoll_create1(0);
    epoll_event event{};
    event.data.fd = server_fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
}

void WebServer::run() {
    epoll_event events[MAX_EVENTS];
    while (running) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num_fds; ++i) {
            if (events[i].data.fd == server_fd) {
                sockaddr_in client_addr{};
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
                if (client_fd < 0) {
                    //std::cerr << "[ERROR] accept() failed, errno: " << errno
                              //<< " (" << strerror(errno) << ")" << std::endl;
                    continue;
                }
                fcntl(client_fd, F_SETFL, O_NONBLOCK);
                epoll_event client_event{};
                client_event.data.fd = client_fd;
                client_event.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
            }
            else if(events[i].events & (EPOLLHUP | EPOLLRDHUP)){
            	int client_fd = events[i].data.fd;
            	//std::cout << "[INFO] Client disconnected: " << client_fd << std::endl;
            	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                close(client_fd);
                continue;
            }
            else
            {
                int client_fd = events[i].data.fd;
                thread_pool.enqueue([this, client_fd] { handleClient(client_fd); });
            }
        }
    }
}

void WebServer::handleClient(int client_fd) {
    char buffer[4096];
    while (true) {
        int len = read(client_fd, buffer, sizeof(buffer) - 1);
        if (len == 0) {
            //std::cout << "[INFO] Client disconnected: " << client_fd << std::endl;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
            close(client_fd);
            return;
        }
        if (len < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  
            } else {
                close(client_fd);
                return;
            }
        }
        buffer[len] = '\0';
    }

    HttpRequest request;
    if (!parseHttpRequest(buffer, request)) {
        sendResponse(client_fd, "400 Bad Request", "Invalid HTTP Request");
        close(client_fd);
        //std::cout<<"http fail"<<std::endl;
        return;
    }

 
    std::string response;
    if (request.method == "GET" && request.path == "/index.html") {
        response = serveHTML("index.html");
    }
    else if (request.method == "POST") {
        response = processRequest(request);
        //std::cout<<"response:"<<response<<std::endl;
    }
    else {
        response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 0\r\n\r\n";
    }

    ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
    if (bytes_sent == -1) {
        //std::cerr << "[ERROR] Failed to send response to client " << client_fd << std::endl;
    } else {
        //std::cout << "[DEBUG] Sent " << bytes_sent << " bytes to client " << client_fd << std::endl;
    }
}

std::string WebServer::processRequest(const HttpRequest& request) {
    if (request.path == "/login") {
        return handleLogin(request);
    }
    else if (request.path == "/update") {
        return handleUpdate(request);
    }
    return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
}


std::string WebServer::serveHTML(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << content.size() << "\r\n\r\n";
    response << content;

    return response.str();
}

std::string WebServer::handleLogin(const HttpRequest& request) {
    std::unordered_map<std::string, std::string> formData;
    std::istringstream bodyStream(request.body);
    std::string token;

    while (std::getline(bodyStream, token, '&')) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            formData[key] = value;
        }
    }

    std::string student_id = formData["student_id"];
    std::string password = formData["password"];

    //std::cout << "[DEBUG] Parsed student_id: " << student_id << ", password: " << password << std::endl;

    bool login_success = db.verifyStudent(student_id, password);
    if (login_success) {
    	//std::cout<<"success"<<std::endl;
        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nLogin Success!";
    } else {
    	//std::cout<<"failed"<<std::endl;
        return "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nLogin Failed!";
    }
}

std::string WebServer::handleUpdate(const HttpRequest& request) {
    std::istringstream bodyStream(request.body);
    std::string student_id, new_signature;
    bodyStream >> student_id;
    std::getline(bodyStream, new_signature);

    bool update_success = db.updateSignature(student_id, new_signature);
    if (update_success) {
        return "HTTP/1.1 200 OK\r\nContent-Length: 16\r\n\r\nUpdate Success!";
    }
    else {
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 14\r\n\r\nUpdate Failed!";
    }
}

void WebServer::sendResponse(int client_fd, const std::string& status, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n\r\n";
    response << body;
    write(client_fd, response.str().c_str(), response.str().size());
}

void WebServer::stop() {
    running = false;
    close(server_fd);
    close(epoll_fd);
    thread_pool.~ThreadPool();
}

