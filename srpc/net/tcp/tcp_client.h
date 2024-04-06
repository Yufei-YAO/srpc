#pragma once
#include <memory>
#include <functional>
#include "common/event_loop.h"
#include "net/net_addr.h"
#include "net/tcp/tcp_buffer.h"
#include "net/tcp/tcp_connection.h"
#include "protocol/string_coder.h"




namespace srpc{

class TcpClient{
public:
    typedef std::shared_ptr<TcpClient> ptr;
    TcpClient(NetAddr::ptr peer_addr);
    ~TcpClient();

    void connect(std::function<void()> done);
    void writeMessage(AbstractProtocol::ptr msg, std::function<void(AbstractProtocol::ptr)> write_done);
    void readMessage(const std::string& req_id, std::function<void(AbstractProtocol::ptr)> read_done);

private:
    EventLoop::ptr m_eventLoop;
    NetAddr::ptr m_peerAddr;
    int m_fd;
    TcpConnection::ptr m_conn;


};






}