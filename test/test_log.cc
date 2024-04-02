#include <iostream>
#include <thread>
#include <vector>
#include "common/log.h"


void test(std::string msg){
    INFOLOG(msg);
}

int main(int argc, char** argv){
    srpc::Logger::InitGlobalLogger();
    srpc::LogEvent::ptr e(new srpc::LogEvent(__FILE__,__LINE__,srpc::LogLevel::INFO,0,1,"hhh"));
    srpc::Logger::ptr g_logger = srpc::Logger::getGlobalLogger();

    g_logger->pushLog(e);
    g_logger->log();

    WARNLOG("test of warn log1");
    std::vector<std::thread> threads;
    for(int i = 0 ;i< 5;++i){
        threads.push_back(std::thread(test, std::to_string(i)));
    }

    for(auto & t : threads){
        t.join();
    }
    WARNLOG("test of warn log2");
    return 0;
}