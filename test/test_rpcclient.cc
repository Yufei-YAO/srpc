#include "srpc.h"
#include <memory>
#include <unistd.h>
#include <iostream>
#include "order.pb.h"













int main(){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
    srpc::IPv4NetAddr::ptr addr = std::make_shared<srpc::IPv4NetAddr>("127.0.0.1",12345);
    srpc::TcpClient cli(addr);

    cli.connect(
        [&cli](){
            INFOLOG("connect");
            srpc::TinyPBProtocol::ptr s = std::make_shared<srpc::TinyPBProtocol>();
            s->setReqID("1111");
            s->m_methodName = "Order.makeOrder";
            makeOrderRequest request;
            request.set_price(100);
            request.set_goods("iPhone");
            if(!request.SerializePartialToString(&s->m_pbData)){
                ERRORLOG("some thing wrong");
                return;
            }
            cli.writeMessage(s,[](srpc::AbstractProtocol::ptr){
                INFOLOG("write success");
            });

            cli.readMessage("1111",[](srpc::AbstractProtocol::ptr p){
                auto k2 = std::dynamic_pointer_cast<srpc::TinyPBProtocol>(p);
                std::cout<<k2->m_reqID<<std::endl;
                std::cout<<k2->m_methodName<<std::endl;
                std::cout<<k2->m_errCode<<std::endl;
                std::cout<<k2->m_errInfo<<std::endl;
                //std::cout<<k2->m_paraseSuccess<<std::endl;
                std::cout<<"-------------------------"<<std::endl;
                makeOrderResponse resp;
                if(!resp.ParseFromString(k2->m_pbData)){
                    ERRORLOG("parse pb data fail");
                    return;
                }
                
                std::cout<<resp.order_id() <<std::endl;
                std::cout<<resp.ret_code() <<std::endl;
                std::cout<<resp.res_info() <<std::endl;

                return;
            });

        }
    );


    pause();

}