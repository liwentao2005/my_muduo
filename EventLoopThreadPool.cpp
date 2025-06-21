#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, const std::string& nameArg)
    : baseLoop_(loop)
    , name_(nameArg)
    , started_(false)
    , numThreads_(0)
    , next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    started_ = true;
    for (int i = 0; i < numThreads_; ++i)
    {
        char buf[64];
        snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
        std::unique_ptr<EventLoopThread> t(new EventLoopThread(cb, std::string(buf)));
        EventLoop* loop = t->startLoop(); // call startLoop() before moving t
        threads_.push_back(std::move(t));
        loops_.push_back(loop);
    }
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if (loops_.empty())
    {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    else
    {
        return loops_;
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = baseLoop_;
    if (!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;
        if (static_cast<size_t>(next_) >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;
}
