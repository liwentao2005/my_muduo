#include "TcpClient.h"

#include "Logger.h"
#include "Connector.h"
#include "EventLoop.h"

#include <stdio.h>

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
//   LOG_TRACE << conn->localAddress().toIpPort() << " -> "
//             << conn->peerAddress().toIpPort() << " is "
//             << (conn->connected() ? "UP" : "DOWN");
  // do not call conn->forceClose(), because some users want to register message callback only.
}

void defaultMessageCallback(const TcpConnectionPtr&,
                                        Buffer* buf,
                                        Timestamp)
{
  buf->retrieveAll();
}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr, const std::string& nameArg)
    : loop_(loop),
      connector_(new Connector(loop, serverAddr)),
      name_(nameArg),
      connectionCallback_(defaultConnectionCallback),
      messageCallback_(defaultMessageCallback),
      retry_(false),
      connect_(true),
      nextConnId_(1)
{
    connector_->setNewConnectionCallback(
        std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
}

static void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
{
  loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

TcpClient::~TcpClient()
{
//   LOG_INFO("TcpClient::~TcpClient["%s"] - connector ", name_.c_str());
  TcpConnectionPtr conn;
  bool unique = false;
  {
    // MutexLockGuard lock(mutex_);
    unique = connection_.unique();
    conn = connection_;
  }
  #if 0
  if (conn)
  {
    // assert(loop_ == conn->getLoop());
    // FIXME: not 100% safe, if we are in different thread
    CloseCallback cb = std::bind(&removeConnection, loop_, std::placeholders::_1);
    loop_->runInLoop(
        std::bind(&TcpConnection::setCloseCallback, conn, cb));
    if (unique)
    {
    //   conn->forceClose();
    }
  }
  else
  {
    connector_->stop();
    // FIXME: HACK
    // loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
  }
  #endif
}

void TcpClient::connect()
{
  // FIXME: check state
  LOG_INFO("TcpClient::connect[%s] - connecting to ", connector_->serverAddress().toIpPort().c_str());
  connect_ = true;
  connector_->start();
}

void TcpClient::disconnect()
{
  connect_ = false;

  {
    // MutexLockGuard lock(mutex_);
    if (connection_)
    {
      connection_->shutdown();
    }
  }
}

void TcpClient::stop()
{
  connect_ = false;
  connector_->stop();
}

static struct sockaddr_in getLocalAddr(int sockfd)
{
    struct sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, (sockaddr *)(&localaddr), &addrlen) < 0)
    {
        LOG_ERROR("sockets::getLocalAddr");
    }
    return localaddr;
}

static struct sockaddr_in getPeerAddr(int sockfd)
{
    struct sockaddr_in peeraddr;
    memset(&peeraddr, 0, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, (sockaddr *)(&peeraddr), &addrlen) < 0)
    {
        LOG_ERROR("sockets::getPeerAddr");
    }
    return peeraddr;
}

void TcpClient::newConnection(int sockfd)
{
//   loop_->assertInLoopThread();
  InetAddress peerAddr(getPeerAddr(sockfd));
  char buf[32];
  snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toIpPort().c_str(), nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;

  InetAddress localAddr(getLocalAddr(sockfd));
  // FIXME poll with zero timeout to double confirm the new connection
  // FIXME use make_shared if necessary
  TcpConnectionPtr conn(new TcpConnection(loop_,
                                          connName,
                                          sockfd,
                                          localAddr,
                                          peerAddr));

  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(
      std::bind(&TcpClient::removeConnection, this, std::placeholders::_1)); // FIXME: unsafe
  {
    // MutexLockGuard lock(mutex_);
    connection_ = conn;
  }
  conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
//   loop_->assertInLoopThread();
//   assert(loop_ == conn->getLoop());

  {
    // MutexLockGuard lock(mutex_);
    // assert(connection_ == conn);
    connection_.reset();
  }

  loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
  if (retry_ && connect_)
  {
    LOG_INFO("TcpClient::connect[%s] - Reconnecting to %s", name_.c_str(), connector_->serverAddress().toIpPort().c_str());
    connector_->restart();
  }
}
