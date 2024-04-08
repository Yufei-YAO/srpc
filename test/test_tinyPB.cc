#include "common/log.h"
#include "net/tcp/tcp_buffer.h"
#include "protocol/tiny_pb_protocol.h"
#include "protocol/tiny_pb_coder.h"
#include "common/config.h"
#include "common/util.h"
#include <iostream>

int main(){
    using namespace srpc;

    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));


    std::vector<AbstractProtocol::ptr> protos;
    TcpBuffer::ptr tbuffer = std::make_shared<TcpBuffer>(128);
    TinyPBCoder coder;
    {
        TinyPBProtocol::ptr p = std::make_shared<TinyPBProtocol>();
        TinyPBCoder coder;
        p->m_reqID="1234";
        p->m_errCode=5678;
        p->m_errInfo="56789";
        p->m_methodName="service.method";
        p->m_pbData="some data to call";
        p->m_pbData.resize(4096);
        p->m_pbData+="sdfds";
        p->m_checkSum = 8908;
        protos.push_back(p);

    }
    //{
        //TinyPBProtocol::ptr p = std::make_shared<TinyPBProtocol>();
        //TinyPBCoder coder;
        //p->m_reqID="81234";
        //p->m_errCode=4678;
        //p->m_errInfo="52534389";
        //p->m_methodName="service.method2";
        //p->m_pbData="some data to call2";
        //p->m_checkSum = 8909;
        //protos.push_back(p);
    //}
    coder.encode(protos,tbuffer);
    //return 0;
    INFOLOG("tbuffer size=%d",tbuffer->readAble());
    INFOLOG("tbuffer=%s",srpc::toHexString(&tbuffer->m_cache[0],tbuffer->readAble()).c_str());
    std::vector<AbstractProtocol::ptr> msgs;
    //tbuffer->m_cache[tbuffer->m_readIndex+26]=8;
    coder.decode(msgs,tbuffer);
    std::cout<<msgs.size()<<std::endl;
    for(auto k : msgs){
        std::cout<<"-------------------------"<<std::endl;
        auto k2 = std::dynamic_pointer_cast<TinyPBProtocol>(k);
        std::cout<<k2->m_reqID<<std::endl;
        std::cout<<k2->m_methodName<<std::endl;
        std::cout<<k2->m_errCode<<std::endl;
        std::cout<<k2->m_errInfo<<std::endl;
        std::cout<<k2->m_pbData<<std::endl;
        std::cout<<k2->m_checkSum<<std::endl;
        //std::cout<<k2->m_paraseSuccess<<std::endl;
        std::cout<<"-------------------------"<<std::endl;
    }


}