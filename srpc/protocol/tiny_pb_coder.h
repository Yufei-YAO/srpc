#pragma once
#include "protocol/abstract_coder.h"
#include "common/bytearray.h"


namespace srpc{

class TinyPBCoder : public AbstractCoder{
public:

    void encode(std::vector<AbstractProtocol::ptr>& in_messages,TcpBuffer::ptr buffer) ;
    void decode(std::vector<AbstractProtocol::ptr>& out_messages,TcpBuffer::ptr buffer);

    ~TinyPBCoder() {}
private:
    ByteArray array; 

};






}