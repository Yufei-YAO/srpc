#include "net/tcp/tcp_buffer.h"
#include "common/log.h"
#include "common/config.h"

int main(){

    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
    srpc::TcpBuffer buffer(128);

    std::vector<char> data;
    for(int i = 0;i<1000;i++){
        data.push_back(char(i%256));
    }
    std::vector<char> data2;
    for(int i = 0;i<2000;i++){
        data2.push_back(char(i%256));
    }
    std::vector<char> ret;
    INFOLOG("readAble = %d",buffer.readAble());
    buffer.writeToBuffer(&data[0],1000);
    INFOLOG("readAble = %d",buffer.readAble());
    buffer.readFromBuffer(ret,396);

    for(int i=0;i<396;i++){
        if(data[i]!=ret[i]){
            ERRORLOG("wrong ");
        }
    }
    buffer.readFromBuffer(ret,128);
    for(int i=0;i<128;i++){
        if(data[396+i]!=ret[i]){
            ERRORLOG("wrong ");
        }
    }

    INFOLOG("readAble = %d",buffer.readAble());

    buffer.readFromBuffer(ret,buffer.readAble());
    INFOLOG("readAble = %d",buffer.readAble());
    buffer.writeToBuffer(&data2[0],data2.size());
    INFOLOG("readAble = %d",buffer.readAble());
    buffer.readFromBuffer(ret,buffer.readAble());
    for(size_t i = 0 ; i < ret.size();++i){
        if(data2[i]!=ret[i]){
            ERRORLOG("wrong ");
        }
    }

    




}