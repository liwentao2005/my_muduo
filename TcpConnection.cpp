#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Logger.h"

#include <errno.h>
#include <functional>

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& nameArg,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    : loop_(loop)
    , name_(nameArg)
    , state_(kCONNECTING)
    , reading_(true)
    , socket_(new Socket(sockfd))
    , channel_(new Channel(loop, sockfd))
    , localAddr_(localAddr)
    , peerAddr_(peerAddr)
    , highWaterMark_(64*1024*1024)
{
    LOG_DEBUG("TcpConnection::ctor[%s] at fd=%d\n", name_.c_str(), sockfd);
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG("TcpConnection::dtor[%s] at fd=%d\n", name_.c_str(), channel_->fd());
}

void TcpConnection::send(const std::string& message)
{
    if (state_ == kCONNECTED)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(message.c_str(), message.size());
        }
        else
        {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message.c_str(), message.size()));
        }
    }
}

void TcpConnection::send(Buffer* buf)
{
  if (state_ == kCONNECTED)
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    }
    else
    {
    //   void (TcpConnection::*fp)(const StringPiece& message) = &TcpConnection::sendInLoop;
    //   loop_->runInLoop(
    //       std::bind(fp,
    //                 this,     // FIXME
    //                 buf->retrieveAllAsString()));
    //                 //std::forward<string>(message)));
    }
  }
}

void TcpConnection::sendInLoop(const void* message, size_t len)
{
    ssize_t nwrote = 0;
    ssize_t remain = len;

    if (state_ == kDISCONNECTED)
    {
        LOG_ERROR("discard data since TcpConnection %s is disconnected!", name_.c_str());
        return;
    }

    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        nwrote = ::write(channel_->fd(), message, len);
        if (nwrote >= 0)
        {
            remain = len - nwrote;
            if (remain == 0 &&  writeCompleteCallback_)
            { 
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            if (errno == EAGAIN)
            {
                // LOG_TRACE << "TcpConnection::send() SO_SNDTIMEO";
            }
            else
            {
                // LOG_SYSERR << "TcpConnection::send()";
            }
        }
    }
}

void TcpConnection::shutdown()
{
    if (state_ == kCONNECTED)
    {
        setState(kDISCONNECTING);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, shared_from_this()));
    }
}

void TcpConnection::shutdownInLoop()
{
    if (!channel_->isWriting())
    {
        socket_->shutdownWrite();
    }
}

void TcpConnection::setTcpNoDelay(bool on)
{
  socket_->setTcpNoDelay(on);
}

void TcpConnection::connectEstablished()
{
    setState(kCONNECTED);
    channel_->tie(shared_from_this());
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    if (state_ == kCONNECTED)
    {
        setState(kDISCONNECTED);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();// del channel from poller
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if (n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        LOG_ERROR("TcpConnection::handleRead\n");
        handleError();
    }
}
void TcpConnection::handleWrite()
{
    if (channel_->isWriting())
    {
        int savedErrno = 0;
        ssize_t n = outputBuffer_.writeFd(channel_->fd(), &savedErrno);
        if (n > 0)
        {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0)
            {
                channel_->disableWriting();
                if (writeCompleteCallback_)
                {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if (state_ == kDISCONNECTING)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR("TcpConnection::handleWrite\n");
        }
    }
    else
    {
        LOG_ERROR("TcpConnection fd=%d is down, no more reading\n", channel_->fd());
    }
}

void TcpConnection::handleClose()
{
    LOG_INFO("TcpConnection::handleClose fd=%d state=%d\n", channel_->fd(), (int)state_);
    setState(kDISCONNECTED);
    channel_->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);
    closeCallback_(guardThis);
}

void TcpConnection::handleError()
{
    int optval;
    socklen_t len = sizeof optval;
    int err = 0;

    if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &len) < 0)
    {
        err = errno;
    }
    else
    {
        err = optval;
    }
    LOG_ERROR("TcpConnection::handleError name=%s - SO_ERROR=%d\n", name_.c_str(), err);
}
