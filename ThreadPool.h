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
    void stopPool(); // ����ֹͣ�̳߳�

private:
    std::vector<std::thread> workers;   // �߳��б�
    std::queue<std::function<void()>> tasks; // �������
    std::mutex queue_mutex;             // ������еĻ�����
    std::condition_variable condition;  // �������������������
    bool stop; // �Ƿ�ֹͣ�̳߳�
};

#endif // THREADPOOL_H
