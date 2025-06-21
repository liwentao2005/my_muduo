#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"

#include <unistd.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)
{
    if (epollfd_ < 0)
    {
        LOG_DEBUG("%d error!", epollfd_);
    }
}

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    // LOG_DEBUG("EPollPoller::poll()");
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        // LOG_DEBUG("time out, nothing happend!");
    }
    else
    {
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_ERROR("EPollPoller::poll() error!");
        }
    }
    return now;
}

void EPollPoller::updateChannel(Channel* channel)
{
    const int index = channel->index();// status in epoll
    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            channels_[fd] = channel;
        }

        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else // added
    {
        int fd = channel->fd();
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}
void EPollPoller::removeChannel(Channel* channel)
{
    int fd = channel->fd();
    channels_.erase(fd);

    int index = channel->index();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}


void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for (int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}
void EPollPoller::update(int operation, Channel* channel)
{
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));

    int fd = channel->fd();

    ev.events = channel->events();
    ev.data.fd = fd;
    ev.data.ptr = channel;

    if(::epoll_ctl(epollfd_, operation, fd, &ev) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl op failed");
        }
        else
        {
            LOG_FATAL("epoll_ctl op");
        }
    }
}
