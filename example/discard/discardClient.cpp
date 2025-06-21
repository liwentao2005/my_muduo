#include <mymuduo/TcpClient.h>

#include <mymuduo/Logger.h>
#include <mymuduo/Thread.h>
#include <mymuduo/Socket.h>
#include <mymuduo/EventLoop.h>
#include <mymuduo/InetAddress.h>

#include <utility>

#include <stdio.h>
#include <unistd.h>

class EchoClient : noncopyable
{
 public:
  EchoClient(EventLoop* loop, const InetAddress& listenAddr, int size)
    : loop_(loop),
      client_(loop, listenAddr, "EchoClient"),
      message_(size, 'H')
  {
    client_.setConnectionCallback(
        std::bind(&EchoClient::onConnection, this, std::placeholders::_1));
    client_.setMessageCallback(
        std::bind(&EchoClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //client_.enableRetry();
  }

  void connect()
  {
    client_.connect();
  }

 private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    LOG_INFO("EchoClient - %s -> %s is %s",
             conn->localAddress().toIpPort().c_str(),
             conn->peerAddress().toIpPort().c_str(),
             conn->connected() ? "UP" : "DOWN");

    if (conn->connected())
    {
      conn->setTcpNoDelay(true);
      conn->send(message_);
    }
    else
    {
    //   loop_->quit();
    }
  }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
  {
    conn->send(buf);
  }

  EventLoop* loop_;
  TcpClient client_;
  string message_;
};

int main(int argc, char* argv[])
{
  LOG_INFO("pid = %d, tid = %d", getpid(), CurrentThread::tid());
  if (argc > 1)
  {
    EventLoop loop;
    InetAddress serverAddr(8088, "127.0.0.1");

    int size = 256;
    if (argc > 2)
    {
      size = atoi(argv[2]);
    }

    EchoClient client(&loop, serverAddr, size);
    client.connect();
    loop.loop();
  }
  else
  {
    printf("Usage: %s host_ip [msg_size]\n", argv[0]);
  }
}
