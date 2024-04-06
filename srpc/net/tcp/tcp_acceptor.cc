#include <string.h>
#include "common/log.h"
#include "common/fd_event_group.h"
#include "net/tcp/tcp_acceptor.h"

namespace srpc{







TcpAcceptor::TcpAcceptor(NetAddr::ptr local_addr){
    if(!local_addr->checkValid()){
        ERRORLOG("TcpAcceptor m_localAddr :%s not valid",m_localAddr->toString().c_str());
        return;
    }
    m_localAddr = local_addr;
    m_family = m_localAddr->getFamily();
    m_listenFd = socket(m_family,SOCK_STREAM,0);
    if(m_listenFd<0){
        ERRORLOG("can not get valid listen fd errno=%d, errstr=%s",errno,strerror(errno));
        exit(0);
    }
    int val = 1;
    if (setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) {
        ERRORLOG("setsockopt REUSEADDR error, errno=%d, error=%s", errno, strerror(errno));
    }
    int rt = ::bind(m_listenFd,local_addr->getAddr(),local_addr->getAddrLen());
    if(rt<0){
        ERRORLOG("can not get valid listen fd errno=%d, errstr=%s",errno,strerror(errno));
        exit(0);
    }

    if(listen(m_listenFd, 1000) != 0) {
        ERRORLOG("listen error, errno=%d, error=%s", errno, strerror(errno));
        exit(0);
    }

}

TcpAcceptor::~TcpAcceptor(){


}

std::pair<int,NetAddr::ptr> TcpAcceptor::accept(){
    if(m_family == AF_INET){
        sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        int peer_fd  = ::accept(m_listenFd,(sockaddr*)&peer_addr,(socklen_t*)&peer_addr_len);
        if(peer_fd < 0){
            ERRORLOG("accept error, errno=%d, error=%s", errno, strerror(errno));
            return {-1,nullptr};
        }
        return {peer_fd,std::make_shared<IPv4NetAddr>(peer_addr)};
    }else{
        return {-1,nullptr};
    }
    
}







}