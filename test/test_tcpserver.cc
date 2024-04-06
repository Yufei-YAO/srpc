#include "srpc.h"
static srpc::EventLoop::ptr eloop = nullptr;
srpc::Semaphore sem;
void testTcpServer(){

    srpc::IPv4NetAddr addr("0.0.0.0",12345);


    srpc::TcpServer server(std::make_shared<srpc::IPv4NetAddr>(addr),1);

    sem.post();
    server.start();


}
int main(){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
           
    //INFOLOG("here");
    std::thread loop_thread(testTcpServer);
    sem.wait();
    pause();
    srpc::TimeEvent::ptr ti1(new srpc::TimeEvent(
        1000,[](){
            INFOLOG("ti1");
        },false
    )) ;
    srpc::TimeEvent::ptr ti2(new srpc::TimeEvent(
        2000,[](){
            INFOLOG("ti2");
        },true
    )) ;
    eloop->addTimerEvent(ti1);
    eloop->addTimerEvent(ti2);
    eloop->wakeup();
    //sleep(1);
    //eloop->wakeup();
    //sleep(2);
    //eloop->wakeup();
    pause();


}