#include "srpc.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <memory.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <semaphore.h>
static srpc::EventLoop::ptr eloop = nullptr;
static sem_t semshore ;
void testEventGroup(){
    
    INFOLOG("start");
    eloop = srpc::EventLoop::GetCurrentEventLoop();
    //INFOLOG("eloop epoll fd:%d",eloop->m_epollFd);
    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1235);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    // 0.0.0.0:1235 代表接受任何ip地址的客户

    bind(sockfd,(struct sockaddr*)&addr,sizeof(sockaddr_in));

    listen(sockfd,5);

    INFOLOG("fdctx fd=%d", sockfd);
    auto fdctx = srpc::FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(sockfd); 
    INFOLOG("fdctx fd=%d now=%d", sockfd, fdctx->getFd());
    fdctx->setNonblock();
    char buf[] = "hello sock\n";
    auto cb = [sockfd,&buf](){
        int clientfd;
        struct sockaddr_in client_addr;
        int socklen;
        std::cout<<"wait accept client"<<std::endl;
        clientfd = accept(sockfd,(struct sockaddr*)&client_addr,(socklen_t*)&socklen);
        auto clientptr = srpc::FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(clientfd);
        std::cout<<"accept client"<<std::endl;
        INFOLOG("accept new client");
        clientptr->setNonblock();
        clientptr->setHandler(srpc::FdEvent::Event::EPOLL_IN,[clientptr](){
            char newbuf[256];
            int pos=0;
            int rt;

            INFOLOG("start read message");
            while(true){
                rt=read(clientptr->getFd(), newbuf+pos,256-pos);
                if(rt==0){
                    INFOLOG("client message: %s",newbuf);
                    INFOLOG("client exit");
                    srpc::EventLoop::GetCurrentEventLoop()->deleteEpollEvent(clientptr);
                    clientptr->close();
                    return;
                }
                if(rt==-1&&errno==EAGAIN){
                    break;
                }
                pos+=rt;
            }
            INFOLOG("client message: %s",newbuf);
        });
        eloop->addEpollEvent(clientptr);


    };
    //INFOLOG("here");
    fdctx->setHandler(srpc::FdEvent::Event::EPOLL_IN,cb); 
    eloop->addEpollEvent(fdctx);
    sem_post(&semshore);
    //INFOLOG("here");
    eloop->loop();

}

int main(){
    srpc::Config::LoadFromYaml("/home/current/srpc/config/config.yaml");
    srpc::Logger::InitGlobalLogger(srpc::StringToLogLevel(srpc::Config::GetGlobalConfig()->c_logLevel));
           
    //INFOLOG("here");
    sem_init(&semshore,0,0);
    std::thread loop_thread(testEventGroup);
    sem_wait(&semshore);

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

    int i;
    std::cin>> i;  
    srpc::TimeEvent::ptr ti3(new srpc::TimeEvent(
        3000,[](){
            INFOLOG("ti3");
        },true
    )) ;
    eloop->addTimerEvent(ti3);

    ti2->setCancelState(true);

    std::cin>> i;  


}