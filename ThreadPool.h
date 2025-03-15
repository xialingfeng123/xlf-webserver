// ThreadPool.h
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();
    void enqueue(std::function<void()> task);
    void stopPool(); // 优雅停止线程池

private:
    std::vector<std::thread> workers;   // 线程列表
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex queue_mutex;             // 任务队列的互斥锁
    std::condition_variable condition;  // 条件变量用于任务调度
    bool stop; // 是否停止线程池
};

#endif // THREADPOOL_H
