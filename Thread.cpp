#include "Thread.h"

std::atomic_int Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false)
    , joined_(false)
    , tid_(0)
    , func_(std::move(func))
    , name_(name)
{
    setDefaultName();
    std::cout  << "Thread start name: " << name_ << std::endl;
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        thread_->detach();
        std::cout  << "Thread exit: " << name_ << std::endl;
    }
}

void Thread::start()
{
    started_ = true;
    sem_t sem;
    sem_init(&sem, 0, 0);
    // Capture 'this' by value to avoid dangling pointer if Thread is destroyed early
    thread_ = std::make_shared<std::thread>([this, &sem](){
        tid_ = CurrentThread::tid();
        sem_post(&sem);
        func_();
    });

    sem_wait(&sem);
    sem_destroy(&sem);
}

void Thread::join()
{
    // 等待线程结束
    if (started_ && !joined_)
    {
        joined_ = true;
        thread_->join();
    }
}

void Thread::setDefaultName()
{
    // 设置线程名
    int num = ++numCreated_;
    if(name_.empty()){
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}
