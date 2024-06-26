#include "srpc.h"
#include <memory>
#include <unistd.h>
#include <iostream>










int main(){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
    srpc::IPv4NetAddr::ptr addr = std::make_shared<srpc::IPv4NetAddr>("127.0.0.1",12345);
    srpc::TcpClient cli(addr);

    //cli.connect(
        //[&cli](){
            //INFOLOG("connect");
            //srpc::StringProtocol::ptr s = std::make_shared<srpc::StringProtocol>();
            //s->setReqID("1111");
            //s->info = "abcdefghijklmn";
            //cli.writeMessage(s,[](srpc::AbstractProtocol::ptr){
                //INFOLOG("write success");
            //});

            //cli.readMessage("1111",[](srpc::AbstractProtocol::ptr p){
                //auto sp = std::dynamic_pointer_cast<srpc::StringProtocol>(p);
                //INFOLOG("read message=%s",sp->info.c_str());
                //return;
            //});

        //}
    //);

    cli.connect(
        [&cli](){
            INFOLOG("connect");
            srpc::TinyPBProtocol::ptr s = std::make_shared<srpc::TinyPBProtocol>();
            s->setReqID("1111");
            s->m_methodName = "service.method";
            s->m_pbData ="some thing important";
            s->m_pbData.resize(4060);
            s->m_pbData+="1245";
            cli.writeMessage(s,[](srpc::AbstractProtocol::ptr){
                INFOLOG("write success");
            });

            cli.readMessage("1111",[](srpc::AbstractProtocol::ptr p){
                auto k2 = std::dynamic_pointer_cast<srpc::TinyPBProtocol>(p);
                std::cout<<k2->m_reqID<<std::endl;
                std::cout<<k2->m_methodName<<std::endl;
                std::cout<<k2->m_errCode<<std::endl;
                std::cout<<k2->m_errInfo<<std::endl;
                std::cout<<k2->m_pbData<<std::endl;
                std::cout<<k2->m_checkSum<<std::endl;
                //std::cout<<k2->m_paraseSuccess<<std::endl;
                std::cout<<"-------------------------"<<std::endl;
                return;
            });

        }
    );


    pause();

}