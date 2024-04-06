#pragma once

#include <memory>
#include "net/net_addr.h"



namespace srpc{


class TcpAcceptor{
public:
    typedef std::shared_ptr<TcpAcceptor> ptr;
    TcpAcceptor(NetAddr::ptr local_addr);
    ~TcpAcceptor();

    std::pair<int,NetAddr::ptr>  accept();

    int getListenFd() const {return m_listenFd;}


private:
    NetAddr::ptr m_localAddr;
    int m_family{-1};
    int m_listenFd{-1};

};








    
}