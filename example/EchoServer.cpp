#include <mymuduo/TcpServer.h>
#include <mymuduo/EventLoop.h>
#include <mymuduo/Logger.h>

#include <iostream>

using namespace std;

int numThreads = 3;

class EchoServer : noncopyable
{
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg = "EchoServer")
    : loop_(loop)
    , server_(loop, listenAddr, nameArg)
    {
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1)
        );
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );

        server_.setThreadNum(numThreads);
    }

    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            LOG_INFO("EchoServer - new connection [%s] from %s",
                     conn->name().c_str(),
                     conn->peerAddress().toIpPort().c_str());
        }
        else
        {
            LOG_INFO("EchoServer - connection [%s] is down",
                     conn->name().c_str());
        }
    }

    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp receiveTime)
    {
        string msg(buf->retrieveAllAsString());
        LOG_INFO("EchoServer - received %zd bytes from connection [%s] at %s: %s",
                 msg.size(),
                 conn->name().c_str(),
                 receiveTime.toString().c_str(),
                 msg.c_str());

        string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n\r\n"; // Close connection after response
        response += "<html><head><title>Muduo HTTP</title></head>";
        response += "<body><h1>Hello " + conn->name() +"say: " + msg + " from Muduo!</h1></body></html>";
        conn->send(response);
        // conn->shutdown(); // close write side
    }

    EventLoop *loop_;
    TcpServer server_;
};

int main()
{
    EventLoop loop;
    InetAddress listenAddr(8088, "127.0.0.1");
    EchoServer server(&loop, listenAddr, "EchoServer");
    server.start();
    LOG_INFO("EchoServer started on %s", listenAddr.toIpPort().c_str());
    loop.loop();

    return 0;
}
