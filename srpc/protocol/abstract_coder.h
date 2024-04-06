#pragma once
#include "net/tcp/tcp_buffer.h"
#include "protocol/abstract_protocol.h"

namespace srpc{


class AbstractCoder {
public:
    typedef std::shared_ptr<AbstractCoder> ptr;
    virtual void encode(std::vector<AbstractProtocol::ptr>& in_messages,TcpBuffer::ptr buffer) = 0;
    virtual void decode(std::vector<AbstractProtocol::ptr>& out_messages,TcpBuffer::ptr buffer) = 0;

    virtual ~AbstractCoder() {}



};

}