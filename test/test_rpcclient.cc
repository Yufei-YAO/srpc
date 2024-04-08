#include "srpc.h"
#include <memory>
#include <unistd.h>
#include <iostream>
#include "order.pb.h"













int main(){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
    srpc::IPv4NetAddr::ptr addr = std::make_shared<srpc::IPv4NetAddr>("127.0.0.1",12345);
    std::shared_ptr<srpc::RpcChannel> channel = std::make_shared<srpc::RpcChannel>(addr);
    Order_Stub stub(channel.get());
    std::shared_ptr<makeOrderRequest> request = std::make_shared<makeOrderRequest>();
    request->set_price(100);
    request->set_goods("iPhone");
    std::shared_ptr<makeOrderResponse> resp = std::make_shared<makeOrderResponse>();

    srpc::RpcController::ptr control = std::make_shared<srpc::RpcController>();
    srpc::RpcClosure::ptr closure = std::make_shared<srpc::RpcClosure>(
        [resp](){
            INFOLOG("rpc success ret_code:%d res_info:%s order_id:%s",resp->ret_code(),resp->res_info().c_str(),resp->order_id().c_str());
            INFOLOG("closure");
        }
    );


    stub.makeOrder(control.get(),request.get(),resp.get(),closure.get());
    INFOLOG("rpc return ret_code:%d res_info:%s order_id:%s",resp->ret_code(),resp->res_info().c_str(),resp->order_id().c_str());


    //cli.connect(
        //[&cli](){
            //INFOLOG("connect");
            //srpc::TinyPBProtocol::ptr s = std::make_shared<srpc::TinyPBProtocol>();
            //s->setReqID("1111");
            //s->m_methodName = "Order.makeOrder";
            //makeOrderRequest request;
            //request.set_price(100);
            //request.set_goods("iPhone");
            //if(!request.SerializePartialToString(&s->m_pbData)){
                //ERRORLOG("some thing wrong");
                //return;
            //}
            //cli.writeMessage(s,[](srpc::AbstractProtocol::ptr){
                //INFOLOG("write success");
            //});

            //cli.readMessage("1111",[](srpc::AbstractProtocol::ptr p){
                //auto k2 = std::dynamic_pointer_cast<srpc::TinyPBProtocol>(p);
                //std::cout<<k2->m_reqID<<std::endl;
                //std::cout<<k2->m_methodName<<std::endl;
                //std::cout<<k2->m_errCode<<std::endl;
                //std::cout<<k2->m_errInfo<<std::endl;
                ////std::cout<<k2->m_paraseSuccess<<std::endl;
                //std::cout<<"-------------------------"<<std::endl;
                //makeOrderResponse resp;
                //if(!resp.ParseFromString(k2->m_pbData)){
                    //ERRORLOG("parse pb data fail");
                    //return;
                //}
                
                //std::cout<<resp.order_id() <<std::endl;
                //std::cout<<resp.ret_code() <<std::endl;
                //std::cout<<resp.res_info() <<std::endl;

                //return;
            //});

        //}
    //);


    //pause();

}