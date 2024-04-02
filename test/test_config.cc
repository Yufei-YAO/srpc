

#include "common/config.h"
#include "common/log.h"
#include "common/util.h"

int main(int argc, char** argv){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));


    INFOLOG("info");
    WARNLOG("warn");

    ERRORLOG("error");

    DEBUGLOG("listenport: %d",srpc::Config::GetGlobalConfig()->c_listenPort);

}