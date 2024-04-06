#include "srpc.h"
#include <memory>
#include <unistd.h>










int main(){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
    srpc::IPv4NetAddr::ptr addr = std::make_shared<srpc::IPv4NetAddr>("127.0.0.1",12345);
    srpc::TcpClient cli(addr);

    cli.connect(
        [&cli](){
            INFOLOG("connect");
            srpc::StringProtocol::ptr s = std::make_shared<srpc::StringProtocol>();
            s->setReqID("1111");
            s->info = "abcdefghijklmn";
            cli.writeMessage(s,[](srpc::AbstractProtocol::ptr){
                INFOLOG("write success");
            });

            cli.readMessage("1111",[](srpc::AbstractProtocol::ptr p){
                auto sp = std::dynamic_pointer_cast<srpc::StringProtocol>(p);
                INFOLOG("read message=%s",sp->info.c_str());
                return;
            });

        }
    );



    pause();

}