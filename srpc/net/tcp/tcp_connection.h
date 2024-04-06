#pragma once
#include <memory>
#include <map>
#include <functional>

#include "common/event_loop.h"
#include "net/net_addr.h"
#include "protocol/abstract_protocol.h"
#include "protocol/abstract_coder.h"



namespace srpc{




class TcpConnection{
public:
    typedef std::shared_ptr<TcpConnection> ptr;
    enum class State{
        NOTCONNECT,
        CONNECT,
        CLOSE,
        HALFCLOSE,
    };
    enum class Type{
        SERVER,
        CLIENT
    };

    TcpConnection(EventLoop::ptr event_loop, int fd, int buffer_size, NetAddr::ptr peer_addr, NetAddr::ptr local_addr, Type type);
    TcpConnection(EventLoop::ptr event_loop, int fd, int buffer_size, NetAddr::ptr peer_addr,  Type type);
    ~TcpConnection();

    void onRead();
    void execute();
    void onWrite();

    void setState(State s){m_state = s;}
    State getState(){return m_state;}

    void clear();
    void shutdown();

    void setType(Type t){ m_type = t;}
    Type getType() {return m_type;}

    void listenRead();
    void listenWrite();

    void pushSendMessage(AbstractProtocol::ptr msg, std::function<void(AbstractProtocol::ptr)> done);
    void pushReadMessage(const std::string & req_id, std::function<void(AbstractProtocol::ptr)> done);



private:
    EventLoop::ptr m_eventLoop;
    int m_fd;
    NetAddr::ptr m_localAddr;
    NetAddr::ptr m_peerAddr;

    TcpBuffer::ptr m_inBuffer;
    TcpBuffer::ptr m_outBuffer;
    AbstractCoder::ptr m_coder;


    std::vector<std::pair<AbstractProtocol::ptr, std::function<void(AbstractProtocol::ptr)>>> m_writeDones;
    
    std::map<std::string,std::function<void(AbstractProtocol::ptr)>> m_readDones;


    State m_state;
    Type m_type;
    





};



}