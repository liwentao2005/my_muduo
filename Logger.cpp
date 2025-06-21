#include "Logger.h"
#include "Timestamp.h"

#include <string>
#include <iostream>

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(LogLevel level)
{
    logLevel_ = level;
}

void Logger::log(std::string msg)
{
    std::cout << Timestamp::now().toString();
    switch (logLevel_)
    {
        case LogLevel::INFO:
        std::cout << "-I-";
        break;
        case LogLevel::ERROR:
        std::cout << "-E-";
        break;
        case LogLevel::FATAL:
        std::cout << "-F-";
        break;
        case LogLevel::DEBUG:
        std::cout << "-D-";
        break;
        default:
        break;
    }
    std::cout << msg << std::endl;
}
