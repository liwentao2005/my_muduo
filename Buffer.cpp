#include "Buffer.h"

#include <sys/uio.h>
#include <unistd.h>

ssize_t Buffer::readFd(int fd, int *saveErrno)
{
    char extrabuf[65536] = {0};
    struct iovec vec[2];

    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writeIndex_;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *saveErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        writeIndex_ += n;
    }
    else
    {
        writeIndex_ = buffer_.size();
        append(extrabuf, n  - writable);
    }

    return n;
}

ssize_t Buffer::writeFd(int fd, int *saveErrno)
{
    size_t readable = readableBytes();
    ssize_t n = ::write(fd, peek(), readable);
    if (n < 0)
    {
        *saveErrno = errno;
        return n;
    }
    retrieve(n);
    return n;
}
