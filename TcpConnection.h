#pragma once

#include "noncopyable.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Buffer.h"
#include "Timestamp.h"

#include <memory>
#include <string>
#include <functional>
#include <atomic>

class Channel;
class EventLoop;
class Socket;

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop, const std::string& nameArg, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const{ return loop_;}
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }

    bool connected() const { return state_ == kCONNECTED; }
    // void send(const void *message, size_t len);

    void send(const std::string& message);
    void sendInLoop(const void* message, size_t len);
    void send(Buffer* message);

    void shutdown();
    void shutdownInLoop();

    void setTcpNoDelay(bool on);

    void setConnectionCallback(const ConnectionCallback& cb){  connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)  { writeCompleteCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb)  { closeCallback_ = cb; }

    void connectEstablished();
    void connectDestroyed();

private:
    enum StateE { kDISCONNECTED, kCONNECTING, kCONNECTED, kDISCONNECTING };
    void setState(StateE s) { state_ = s; }

    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    EventLoop* loop_;
    const std::string name_;
    std::atomic<StateE> state_;
    bool reading_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    const InetAddress localAddr_;
    const InetAddress peerAddr_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback closeCallback_;

    size_t highWaterMark_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;
};
