#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty()) return; // 退出线程
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
            });
    }
}

ThreadPool::~ThreadPool() {
    stopPool();
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(std::move(task));
    }
    condition.notify_one(); // 唤醒一个工作线程
}

void ThreadPool::stopPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all(); // 唤醒所有线程
    for (std::thread& worker : workers) {
        if (worker.joinable()) worker.join();
    }
}
