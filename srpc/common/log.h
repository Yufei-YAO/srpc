#pragma once
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <sys/time.h>
#include "common/util.h"





#define DEBUGLOG(str,...)\
    srpc::Logger::getGlobalLogger()->pushLog(srpc::LogEvent::ptr(new srpc::LogEvent(__FILE__,__LINE__,srpc::LogLevel::DEBUG,srpc::getProcessID(),srpc::getThreadID(),srpc::formatString(str,##__VA_ARGS__))));\
    srpc::Logger::getGlobalLogger()->log();

#define INFOLOG(str,...)\
    srpc::Logger::getGlobalLogger()->pushLog(srpc::LogEvent::ptr(new srpc::LogEvent(__FILE__,__LINE__,srpc::LogLevel::INFO,srpc::getProcessID(),srpc::getThreadID(),srpc::formatString(str,##__VA_ARGS__))));\
    srpc::Logger::getGlobalLogger()->log();

#define WARNLOG(str,...)\
    srpc::Logger::getGlobalLogger()->pushLog(srpc::LogEvent::ptr(new srpc::LogEvent(__FILE__,__LINE__,srpc::LogLevel::WARN,srpc::getProcessID(),srpc::getThreadID(),srpc::formatString(str,##__VA_ARGS__))));\
    srpc::Logger::getGlobalLogger()->log();

#define ERRORLOG(str,...)\
    srpc::Logger::getGlobalLogger()->pushLog(srpc::LogEvent::ptr(new srpc::LogEvent(__FILE__,__LINE__,srpc::LogLevel::ERROR,srpc::getProcessID(),srpc::getThreadID(),srpc::formatString(str,##__VA_ARGS__))));\
    srpc::Logger::getGlobalLogger()->log();
namespace srpc{
template <typename... Args>
std::string formatString(const std::string& str, Args&&... args){
    int size = snprintf(nullptr, 0, str.c_str(), args...);

    std::string result;
    if (size > 0) {
        result.resize(size);
        snprintf(&result[0], size + 1, str.c_str(), args...);
    }

  return result;
}

enum class LogLevel{
    UNKNOWN,
    DEBUG,
    INFO,
    WARN,
    ERROR
};


class LogEvent{
friend class Logger;
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::string file, uint32_t line, LogLevel level, int32_t pid,
            int32_t threadID, std::string msg = "");
    std::string toString() const;

    //void operator<<(const std::string msg);
private:
    std::string m_file;
    uint32_t m_line;
    LogLevel m_level;
    int32_t m_pid;
    int32_t m_threadID;
    struct timeval m_time;
    //uint64_t m_ms;
    std::string m_msg;

};


class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(LogLevel level = LogLevel::DEBUG);

    void pushLog(LogEvent::ptr log);
    void log();

    void setLevel(LogLevel level){m_level = level;}
    LogLevel getLevel() const {return m_level;}
public:
    static void InitGlobalLogger(LogLevel level = LogLevel::DEBUG);
    static Logger::ptr getGlobalLogger();
private:
    LogLevel m_level;
    std::vector<LogEvent::ptr> m_logs;
    std::mutex m_mutex;
};


LogLevel StringToLogLevel(const std::string& l);

}