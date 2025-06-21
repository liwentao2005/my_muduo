#pragma once

#include <string>
#include <cstring>

#include "noncopyable.h"

#define filename(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x

#if 1

#define LOG_INFO(logfmt, ...)                                  \
    do                                                         \
    {                                                          \
        Logger &logger = Logger::instance();                   \
        logger.setLogLevel(LogLevel::INFO);                    \
        char buf[1024] = {0};                                  \
        snprintf(buf, sizeof buf, "%s-%d-" logfmt,             \
                 filename(__FILE__), __LINE__, ##__VA_ARGS__); \
        logger.log(buf);                                       \
    } while (0)

#define LOG_ERROR(logfmt, ...)                                 \
    do                                                         \
    {                                                          \
        Logger &logger = Logger::instance();                   \
        logger.setLogLevel(LogLevel::ERROR);                   \
        char buf[1024] = {0};                                  \
        snprintf(buf, sizeof buf, "%s-%d-" logfmt,             \
                 filename(__FILE__), __LINE__, ##__VA_ARGS__); \
        logger.log(buf);                                       \
    } while (0)

#define LOG_DEBUG(logfmt, ...)                                 \
    do                                                         \
    {                                                          \
        Logger &logger = Logger::instance();                   \
        logger.setLogLevel(LogLevel::DEBUG);                   \
        char buf[1024] = {0};                                  \
        snprintf(buf, sizeof buf, "%s-%d-" logfmt,             \
                 filename(__FILE__), __LINE__, ##__VA_ARGS__); \
        logger.log(buf);                                       \
    } while (0)

    #define LOG_FATAL(logfmt, ...)                                 \
    do                                                         \
    {                                                          \
        Logger &logger = Logger::instance();                   \
        logger.setLogLevel(LogLevel::FATAL);                   \
        char buf[1024] = {0};                                  \
        snprintf(buf, sizeof buf, "%s-%d-" logfmt,             \
            filename(__FILE__), __LINE__, ##__VA_ARGS__); \
            logger.log(buf);                                       \
            exit(1);                                               \
        } while (0)
        
#else

#define LOG_INFO(logfmt, ...)
#define LOG_ERROR(logfmt, ...)
#define LOG_DEBUG(logfmt, ...)
#define LOG_FATAL(logfmt, ...)                                 \
    do                                                         \
    {                                                          \
        Logger &logger = Logger::instance();                   \
        logger.setLogLevel(LogLevel::FATAL);                   \
        char buf[1024] = {0};                                  \
        snprintf(buf, sizeof buf, "%s-%d-" logfmt,             \
            filename(__FILE__), __LINE__, ##__VA_ARGS__); \
            logger.log(buf);                                       \
            exit(1);                                               \
        } while (0)

#endif

enum class LogLevel
{
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

class Logger : noncopyable
{
public:
    static Logger &instance();
    void setLogLevel(LogLevel level);

    void log(std::string msg);

private:
    LogLevel logLevel_;
    Logger() {}
};
