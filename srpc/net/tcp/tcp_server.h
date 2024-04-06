#pragma once
#include <memory>
#include <set>
#include "common/event_loop.h"
#include "common/io_thread_pool.h"
#include "net/net_addr.h"
#include "net/tcp/tcp_acceptor.h"
#include "net/tcp/tcp_connection.h"

namespace srpc{


class TcpServer{

public:
    typedef std::shared_ptr<TcpServer> ptr;
    TcpServer(NetAddr::ptr local_addr, int workers = 0);
    ~TcpServer();


    void start();
private:
    void onAccept();


private:

    NetAddr::ptr m_localAddr;
    TcpAcceptor::ptr m_acceptor;
    EventLoop::ptr m_mainLoop;
    IOThreadPool::ptr m_ioThreads;
    FdEvent::ptr m_listenFd;
    std::set<TcpConnection::ptr> m_connections;



};





}