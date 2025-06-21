#pragma once

#include "noncopyable.h"

#include <vector>
#include <cstddef>
#include <string>

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initSize = kInitialSize)
        : buffer_(kCheapPrepend + initSize)
        , readIndex_(kCheapPrepend)
        , writeIndex_(kCheapPrepend)
        { }

    size_t readableBytes() const { return writeIndex_ - readIndex_; }
    size_t writableBytes() const { return buffer_.size() - writeIndex_; }
    size_t prependableBytes() const { return readIndex_; }

    const char* peek() const { return begin() + readIndex_; }

    void retrieve(size_t len) {
        if (len < readableBytes()) {
            readIndex_ += len;
        } else {
            retrieveAll();
        }
    }

    void retrieveAll() {
        readIndex_ = kCheapPrepend;
        writeIndex_ = kCheapPrepend;
    }

    std::string retrieveAsString(size_t len) {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
    }

    void append(const std::string& str) {
        append(str.data(), str.length());
    }

    void append(const char* str, size_t len) {
        ensureWritableBytes(len);
        std::copy(str, str + len, beginWrite());
    }

    ssize_t readFd(int fd, int* saveErrno);
    ssize_t writeFd(int fd, int* saveErrno);

private:
    char* begin() { return &*buffer_.begin(); }
    const char* begin() const { return &*buffer_.begin(); }

    char* beginWrite() { return begin() + writeIndex_; }

    const char* beginWrite() const { return begin() + writeIndex_; }
    
    void makeSpace(size_t len) {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writeIndex_ + len);
        } else {
            size_t readable = readableBytes();
            std::copy(begin() + readIndex_, begin() + writeIndex_, begin() + kCheapPrepend);
            readIndex_ = kCheapPrepend;
        }
    }

    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;
};
