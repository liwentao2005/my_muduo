#include <mymuduo/TcpServer.h>
#include <iostream>

using namespace std;

int numThreads = 3;

class DoipServer : noncopyable
{
public:
    DoipServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg = "DoipServer")
    : loop_(loop)
    , server_(loop, listenAddr, nameArg)
    {
        server_.setConnectionCallback(
            std::bind(&DoipServer::onConnection, this, std::placeholders::_1)
        );
        server_.setMessageCallback(
            std::bind(&DoipServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
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
            LOG_INFO("DoipServer - new connection [%s] from %s",
                     conn->name().c_str(),
                     conn->peerAddress().toIpPort().c_str());
        }
        else
        {
            LOG_INFO("DoipServer - connection [%s] is down",
                     conn->name().c_str());
        }
    }

    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp receiveTime)
    {
        string msg(buf->retrieveAllAsString());
        LOG_INFO("DoipServer - received %zd bytes from connection [%s] at %s: %s",
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
        conn->shutdown(); // close write side
    }

    EventLoop *loop_;
    TcpServer server_;
};

int main()
{
    EventLoop loop;
    InetAddress listenAddr(8088, "172.31.71.50");
    DoipServer server(&loop, listenAddr, "DoipServer");
    server.start();
    LOG_INFO("DoipServer started on %s", listenAddr.toIpPort().c_str());
    loop.loop();

    return 0;
}
