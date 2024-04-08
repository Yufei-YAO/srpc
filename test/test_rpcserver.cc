#include "srpc.h"
#include <memory>
#include <unistd.h>
#include <iostream>
#include "order.pb.h"
#include <google/protobuf/service.h>

class OrderImpl : public Order{
public:
    typedef std::shared_ptr<OrderImpl> ptr;
    void makeOrder(google::protobuf::RpcController* controller,
                       const ::makeOrderRequest* request,
                       ::makeOrderResponse* response,
                       ::google::protobuf::Closure* done){
        if(request->price() < 10){
            response->set_ret_code(-1);
            response->set_res_info("not enough money");
            return;
        }
        response->set_ret_code(222);
        response->set_res_info("suce");
        response->set_order_id("22222222");
        if(done){
            done->Run();
        }
    }

};







int main(){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
    srpc::IPv4NetAddr::ptr addr = std::make_shared<srpc::IPv4NetAddr>("0.0.0.0",srpc::Config::GetGlobalConfig()->c_listenPort);
    OrderImpl::ptr oim= std::make_shared<OrderImpl>();
    srpc::RpcDispatcher::GetRpcDispatcher()->registerService(oim);
    srpc::TcpServer server(addr);

    server.start();
    pause();

}