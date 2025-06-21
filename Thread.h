#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <semaphore.h>
#include <memory>

#include "CurrentThread.h"
#include "noncopyable.h"

using namespace std;

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, const std::string &name = std::string());
    ~Thread();

    void start();
    void join();

    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string &name() const { return name_; }
    static int numCreated() { return numCreated_; }

    inline bool is_calling_thread() const noexcept
    {
        return tid() == CurrentThread::tid();
    }

private:

    void setDefaultName();

    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    static std::atomic_int numCreated_;

};
