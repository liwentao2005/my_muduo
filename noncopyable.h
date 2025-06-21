#pragma once

/**
 * 
 *
 * 禁止拷贝构造函数和赋值操作符
 * 
 * 派生类可以可以继承可以构造和析构
 * 
 */
class noncopyable {
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};
