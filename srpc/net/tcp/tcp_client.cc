#include <string.h>
#include "common/log.h"
#include "common/fd_event_group.h"
#include "net/tcp/tcp_client.h"





namespace srpc{




TcpClient::TcpClient(NetAddr::ptr peer_addr){
    if(!peer_addr->checkValid()){
        ERRORLOG("TcpClient initial with wrong peer_addr:%d",peer_addr->toString());
    }
    m_peerAddr = peer_addr;
    m_eventLoop = EventLoop::GetCurrentEventLoop();
    m_fd = socket(m_peerAddr->getFamily(),SOCK_STREAM,0);
    if(m_fd < 0){
        ERRORLOG("Create Client socket failed with errno=%d, errstr=%s",errno,strerror(errno));
        return;
    }
    FdEvent::ptr fdp =  FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_fd);
    m_conn = std::make_shared<TcpConnection>(TcpConnection(m_eventLoop,m_fd,128,peer_addr,TcpConnection::Type::CLIENT));
}
TcpClient::~TcpClient(){
    DEBUGLOG("~TcpClient");
}

void TcpClient::connect(std::function<void()> done){
    int rt = ::connect(m_fd,m_peerAddr->getAddr(),m_peerAddr->getAddrLen());
    if(rt == 0){
        done();
    }
    else if(errno == EINPROGRESS){
        FdEvent::ptr fdp =  FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_fd);
        auto errCallback = [](){
            ERRORLOG("TcpClient connect fail");
            return;
        };
        fdp->setHandler(FdEvent::Event::EPOLL_OUT,
            [this,fdp,done](){
                int error = 0;
                socklen_t error_len = sizeof(error);
                getsockopt(fdp->getFd(), SOL_SOCKET, SO_ERROR, &error, &error_len);
                bool is_connect_succ = false;
                if (error == 0) {
                    DEBUGLOG("connect [%s] sussess", m_peerAddr->toString().c_str());
                    is_connect_succ = true;
                    struct sockaddr_in addr;
                    socklen_t addrLen = sizeof(addr);
                    if (getsockname(fdp->getFd(), (struct sockaddr*)&addr, &addrLen) == -1) {
                        //close(sockfd);
                        //return 1;
                    }

                    IPv4NetAddr::ptr ar = std::make_shared<IPv4NetAddr>(addr);
                    m_conn->setLocalAddr(ar);
                    m_conn->setState(TcpConnection::State::CONNECT);
 
                } else {
                    ERRORLOG("connect errror, errno=%d, error=%s", errno, strerror(errno));
                }
                // 连接完后需要去掉可写事件的监听，不然会一直触发
                fdp->cancelHandler(FdEvent::Event::EPOLL_OUT);
                m_eventLoop->addEpollEvent(fdp);

                if (is_connect_succ && done) {
                  done();
                }
            } 
            ,errCallback 
        );

        m_eventLoop->addEpollEvent(fdp);
        if(!m_eventLoop->isLooping()){
            m_eventLoop->loop();
        }
        



    } else{
        ERRORLOG("TcpClient connect errno=%d errstr=%s",errno,strerror(errno));
        return;
    }

    //m_eventLoop->loop();


}
void TcpClient::writeMessage(AbstractProtocol::ptr msg, std::function<void(AbstractProtocol::ptr)> write_done){
    if(m_conn == nullptr){
        ERRORLOG("TcpClient writeMessage in a null connection");
        return ;
    }
    m_conn->pushSendMessage(msg,write_done);
    m_conn->listenWrite();


}
void TcpClient::readMessage(const std::string& req_id, std::function<void(AbstractProtocol::ptr)> read_done){
    if(m_conn == nullptr){
        ERRORLOG("TcpClient readMessage in a null connection");
        return ;
    }
    m_conn->pushReadMessage(req_id,read_done);
    m_conn->listenRead();

}









}