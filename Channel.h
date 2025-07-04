#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <functional>
#include <memory>
#include <string>

using std::string;

class EventLoop;

/**
 * @brief fd event
 * 
 */
class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    void tie(const std::shared_ptr<void>& obj) { tie_ = obj; tied_ = true; }

    int fd() const { return fd_; }
    int events() const { return events_; }
    void setRevents(int revt) { revents_ = revt; }
    
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }
    
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() const { return index_; }
    void setIndex(int idx) { index_ = idx; }

    // for debug
    string reventsToString() const;
    string eventsToString() const;

    EventLoop* ownerLoop() { return loop_; }
    void remove();

private:
    static string eventsToString(int fd, int ev);
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;
    bool addedToLoop_;
    bool eventHandling_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};