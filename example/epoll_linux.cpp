#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <memory>

#define MAX_EVENTS 10

class MyCar : public std::enable_shared_from_this<MyCar>
{
public:
    MyCar()
    {
        name_ = "MyCar";
    }
    MyCar(std::string name = "MyCar")
        : name_(name)
    {
    }
    std::shared_ptr<MyCar> get_ptr()
    {
        return shared_from_this();
    }
    ~MyCar()
    {
        std::cout << "free ~Mycar() name=" << name_ << std::endl;
    }
    std::string name()
    {
        return name_;
    }
    std::string name() const
    {
        return name_;
    }


private:
    std::string name_;
};

std::ostream &operator<<(std::ostream &os, const MyCar &car)
{
    os << "MyCar name = " << car.name();
    return os;
}

#define TEST_SHARED_PTR 1

int main()
{
#if TEST_SHARED_PTR
    MyCar *_myCar = new MyCar("MyCar000");
#if 1 // OK
#if 0
    std::shared_ptr<MyCar> _myCar1(_myCar);
    std::shared_ptr<MyCar> _myCar2 = _myCar1->get_ptr();
#else // best
    // MyCar *_myCarCar = new MyCar();
    std::shared_ptr<MyCar> _myCarCar = std::make_shared<MyCar>("MyCar001");
    std::shared_ptr<MyCar> _myCar1 = _myCarCar->get_ptr();
    std::shared_ptr<MyCar> _myCar2 = _myCarCar->get_ptr();
#endif
#else // error
    #if 0
    std::shared_ptr<MyCar> _myCar1(_myCar);
    std::shared_ptr<MyCar> _myCar2(_myCar);
    #else
        std::shared_ptr<MyCar> _myCar1 = _myCar->get_ptr();
        std::shared_ptr<MyCar> _myCar2 = _myCar->get_ptr();
    #endif
#endif
    std::cout << _myCar1.use_count() << std::endl;
    std::cout << _myCar2.use_count() << std::endl;

    std::cout << *_myCar << std::endl;
    std::cout << *_myCar1.get() << std::endl;
    delete _myCar;
    _myCar = nullptr;
    return 0;

#else // epoll test TEST_SHARED_PTR

    int epfd, sockfd, n, clientfd;
    struct sockaddr_in addr;
    struct epoll_event ev, events[MAX_EVENTS];

    // 创建epoll实例
    epfd = epoll_create1(0);
    if (epfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // 创建socket并绑定到指定端口
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, SOMAXCONN) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 将socket添加到epoll实例中，监听EPOLLIN事件
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
        perror("epoll_ctl: sockfd");
        exit(EXIT_FAILURE);
    }

    // 等待事件
    while (1)
    {
        n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (n == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++)
        {
            if (events[i].data.fd == sockfd)
            {
                // 接受新连接
                clientfd = accept(sockfd, NULL, NULL);
                if (clientfd == -1)
                {
                    perror("accept");
                    continue;
                }

                // 将新连接添加到epoll实例中，监听EPOLLIN事件
                ev.events = EPOLLIN | EPOLLET; // 使用边缘触发模式
                ev.data.fd = clientfd;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev) == -1)
                {
                    perror("epoll_ctl: clientfd");
                    close(clientfd);
                    continue;
                }

                printf("New connection accepted, fd: %d\n", clientfd);
            }
            else
            {
                // 处理已连接socket上的事件
                int fd = events[i].data.fd;
                char buf[1024];
                ssize_t count;

                printf("1.---EPOLLIN event on fd %d.\n", events[i].events);
                if ((count = read(fd, buf, sizeof(buf))) > 0)
                {
                    // 处理接收到的数据
                    printf("20.Received data on fd %d: %s\n", fd, buf);
                }
                else
                {
                    printf("21.Received data on fd %d, cnt: %d.\n", fd, (int)count);
                }

                if (count == -1 && errno != EAGAIN)
                {
                    // 读取错误，关闭socket
                    perror("read");
                    close(fd);
                }

                // 检查是否收到EPOLLIN事件（在关闭socket时可能会多次触发）
                if (events[i].events & EPOLLIN)
                {
                    // printf("EPOLLIN event on fd %d\n", fd);
                }
            }
        }
    }

    close(epfd);
    close(sockfd);
    return 0;
#endif
}

