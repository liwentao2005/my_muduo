#include "Connector.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Logger.h"

#include <errno.h>
#include <cassert>
#include <sys/socket.h>
#include <string.h>

const int Connector::kMaxRetryDelayMs = 30 * 1000;

Connector::Connector(EventLoop *loop, const InetAddress &serverAddr)
    : loop_(loop), serverAddr_(serverAddr), connect_(false), state_(kDisconnected), retryDelayMs_(kInitRetryDelayMs)
{
}

Connector::~Connector()
{
    assert(!channel_);
}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
    if (connect_)
    {
        connect();
    }
    else
    {
        LOG_DEBUG("Connector do not connect");
    }
}

void Connector::stop()
{
    connect_ = false;
    loop_->queueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop()
{
    if (state_ == kConnecting)
    {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

static int createNonblocking()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0)
    {
        LOG_FATAL("Acceptor::Acceptor() %s in %s:%d", strerror(errno), __FILE__, __LINE__);
    }
    return sockfd;
}

void Connector::connect()
{
    int sockfd = createNonblocking();
    int ret = ::connect(sockfd, (sockaddr *)serverAddr_.getSockAddr(), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        LOG_INFO("connect OK in Connector::startInLoop :%d.", savedErrno);
        connecting(sockfd);
        break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
        LOG_ERROR("connect error in Connector::startInLoop :%d.", savedErrno);
        retry(sockfd);
        break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        LOG_ERROR("connect error in Connector::startInLoop :%d.", savedErrno);
        ::close(sockfd);
        break;

    default:
        LOG_ERROR("Unexpected error in Connector::startInLoop :%d.", savedErrno);
        close(sockfd);
        // connectErrorCallback_();
        break;
    }
}

void Connector::restart()
{
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}
void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(std::bind(&Connector::handleError, this));
    channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();

    loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}
void Connector::resetChannel()
{
    channel_.reset();
}

static int getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}

static struct sockaddr_in6 getLocalAddr(int sockfd)
{
    struct sockaddr_in6 localaddr;
    memset(&localaddr, 0, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, (sockaddr *)(&localaddr), &addrlen) < 0)
    {
        LOG_ERROR("sockets::getLocalAddr");
    }
    return localaddr;
}

static struct sockaddr_in6 getPeerAddr(int sockfd)
{
    struct sockaddr_in6 peeraddr;
    memset(&peeraddr, 0, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, (sockaddr *)(&peeraddr), &addrlen) < 0)
    {
        LOG_ERROR("sockets::getPeerAddr");
    }
    return peeraddr;
}

static bool isSelfConnect(int sockfd)
{
    struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
    struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
    if (localaddr.sin6_family == AF_INET)
    {
        const struct sockaddr_in *laddr4 = reinterpret_cast<struct sockaddr_in *>(&localaddr);
        const struct sockaddr_in *raddr4 = reinterpret_cast<struct sockaddr_in *>(&peeraddr);
        return laddr4->sin_port == raddr4->sin_port && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    }
    else if (localaddr.sin6_family == AF_INET6)
    {
        return localaddr.sin6_port == peeraddr.sin6_port && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr) == 0;
    }
    else
    {
        return false;
    }
}

void Connector::handleWrite()
{
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = getSocketError(sockfd);
        if (err)
        {
            LOG_DEBUG("Connector::handleWrite - SO_ERROR = %d - %s", err, strerror(err));
            retry(sockfd);
        }
        else if (isSelfConnect(sockfd))
        {
            LOG_DEBUG("Connector::handleWrite - Self connect");
            retry(sockfd);
        }
        else
        {
            setState(kConnected);
            if (connect_)
            {
                newConnectionCallback_(sockfd);
            }
            else
            {
                ::close(sockfd);
            }
        }
    }
    else
    {
        // what happened?
        assert(state_ == kDisconnected);
    }
}

void Connector::handleError()
{
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = getSocketError(sockfd);
        LOG_ERROR("Connector::handleError - SO_ERROR = %d %s\n", err, strerror(err));
        retry(sockfd);
    }
}

void Connector::retry(int sockfd)
{
    ::close(sockfd);
    setState(kDisconnected);
    if (connect_)
    {
        LOG_INFO("Connector::retry - Retry connecting to %s in %d milliseconds. \n", serverAddr_.toIpPort().c_str(), retryDelayMs_);
        // loop_->runAfter(retryDelayMs_/1000.0,
        //                 std::bind(&Connector::startInLoop, shared_from_this()));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
    }
    else
    {
        LOG_DEBUG("Connector::retry - do not connect");
    }
}
