#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "ThreadPool.h"
#include "HttpParser.h"
#include "StudentDB.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <csignal>

const int SERVER_PORT = 8080;
const int MAX_EVENTS = 512;
const int THREAD_POOL_SIZE = 8;

class WebServer {
public:
    WebServer(int port, int num_threads);
    ~WebServer();
    void run();
    void stop();

private:
    int server_fd;
    int epoll_fd;
    ThreadPool thread_pool;
    bool running;
    StudentDB& db;  

    void handleClient(int client_fd);
    void setupServer(int port);


    std::string processRequest(const HttpRequest& request);
    std::string handleLogin(const HttpRequest& request);
    std::string handleUpdate(const HttpRequest& request);
    std::string serveHTML(const std::string& filename);
    void sendResponse(int client_fd, const std::string& status, const std::string& body);
};

#endif 

