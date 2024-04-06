#include "common/log.h"
#include "common/fd_event_group.h"
#include "net/tcp/tcp_server.h"










namespace srpc{



TcpServer::TcpServer(NetAddr::ptr local_addr, int workers){
    m_mainLoop = EventLoop::GetCurrentEventLoop(); 
    m_localAddr = local_addr;    
    m_acceptor = std::make_shared<TcpAcceptor>(local_addr);
    m_listenFd = FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_acceptor->getListenFd());
    m_listenFd->setNonblock();
    m_listenFd->setHandler(FdEvent::Event::EPOLL_IN,std::bind(&TcpServer::onAccept,this));
    m_mainLoop->addEpollEvent(m_listenFd);
    m_ioThreads = std::make_shared<IOThreadPool>(workers);
}
TcpServer::~TcpServer(){


}


void TcpServer::start(){
    m_ioThreads->start();
    m_mainLoop->loop();

}
void TcpServer::onAccept(){
    std::pair<int,NetAddr::ptr> client =  m_acceptor->accept();
    TcpConnection::ptr conn = std::make_shared<TcpConnection>(m_ioThreads->getEventLoop(),client.first,128,client.second,m_localAddr,TcpConnection::Type::SERVER);

    conn->listenRead();
    m_connections.insert(conn);
    INFOLOG("TcpServer succ get client, fd=%d", client.first);

}
}