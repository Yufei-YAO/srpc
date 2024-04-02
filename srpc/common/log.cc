#include <iostream>
#include <string.h>
#include <sstream>
#include <memory>
#include "common/log.h"


namespace srpc{

static Logger::ptr g_logger = nullptr;

void Logger::InitGlobalLogger(LogLevel level ){
    g_logger.reset(new Logger(level)); 
}
Logger::ptr Logger::getGlobalLogger(){
    if(!g_logger) {
        exit(0);
    }
    return g_logger;
}
LogLevel StringToLogLevel(const std::string& l){
#define XX(level)\
    if(!strcasecmp(#level,l.c_str())){ \
       return LogLevel::level;  \
    }

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);

    return LogLevel::UNKNOWN;

#undef XX

}

static std::string LevelToString(const LogLevel level){

    switch (level)
    {
#define XX(level)\
    case LogLevel::level: \
        return #level; 
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
#undef XX
    default:
        return "UNKNOWN";
    }

}

LogEvent::LogEvent(std::string file, uint32_t line, LogLevel level, int32_t pid,
            int32_t threadID, std::string msg)
            :m_file(file)
            ,m_line(line)
            ,m_level(level)
            ,m_pid(pid)
            ,m_threadID(threadID)
            ,m_msg(msg){
    gettimeofday(&m_time, nullptr);


}


std::string LogEvent::toString() const{
    struct tm tm;
    localtime_r(&(m_time.tv_sec), &tm);
    char buf[64];
    strftime(&buf[0], 64, "%y-%m-%d %H:%M:%S", &tm);
    int ms = m_time.tv_usec / 1000; 

    std::string time_str(buf);
    time_str = time_str + "." + std::to_string(ms);
    std::stringstream ss;

    ss << "[" << LevelToString(m_level) << "]\t"
    << "[" << time_str << "]\t"
    <<  m_file << ":" << m_line << "\t"
    << "[" << m_pid << ":" << m_threadID << "]\t";
    if(!m_msg.empty()){
        ss<< m_msg;
    }
    ss<<std::endl;
    return ss.str();

}
Logger::Logger(LogLevel level)
    :m_level(level){

}

void Logger::pushLog(LogEvent::ptr log){
    if(log->m_level < m_level) return;
    std::lock_guard<std::mutex> l(m_mutex);
    m_logs.push_back(log);
    //if()
}
void Logger::log(){
    // std::unique_lock<std::mutex> l(m_mutex);
    // std::vector<LogEvent::ptr> tmp;
    // tmp.swap(m_logs);
    // l.unlock();
    std::lock_guard<std::mutex> l(m_mutex);
    for(auto & k : m_logs){
        std::cout<<k->toString();
    }
    m_logs.clear();
}





}