#include "Poller.h"
#include "EventLoop.h"
#include "EPollPoller.h"

#include <stdlib.h>

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    if (::getenv("USE_POLLER"))
    {
        return nullptr;
    }
    else
    {
        return new EPollPoller(loop);
    }
}