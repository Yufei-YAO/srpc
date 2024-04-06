#include "srpc.h"
#include <iostream>
using namespace std;
static srpc::EventLoop::ptr eloop = nullptr;
static sem_t semshore ;
void testConnection(){

    srpc::IPv4NetAddr addr("0.0.0.0",12345);

    srpc::TcpAcceptor acc(std::make_shared<srpc::IPv4NetAddr>(addr));

    char buf[] = "hello sock\n";
    while(true){
        std::cout<<"wait accept client"<<std::endl;
        auto [l,p] = acc.accept();
        std::cout<<"accept client"<<std::endl;
        write(l,buf,sizeof(buf));
        close(l);
    }

}
int main(){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
           
    //INFOLOG("here");
    sem_init(&semshore,0,0);
    std::thread loop_thread(testConnection);
    sem_wait(&semshore);



}