#pragma once
#include "protocol/abstract_coder.h"
#include "protocol/abstract_protocol.h"


namespace srpc{


class StringProtocol : public AbstractProtocol{
public:
    typedef std::shared_ptr<StringProtocol> ptr;
public:
    std::string info;

};

class StringCoder : public AbstractCoder{
public:
    void encode(std::vector<AbstractProtocol::ptr>& in_messages,TcpBuffer::ptr buffer){
        for(auto &k : in_messages){
            StringProtocol::ptr pro = std::dynamic_pointer_cast<StringProtocol>(k);
            buffer->writeToBuffer(pro->info.c_str(),pro->info.size());
        }

    }
    void decode(std::vector<AbstractProtocol::ptr>& out_messages,TcpBuffer::ptr buffer){
        std::vector<char> in; 

        buffer->readFromBuffer(in,buffer->readAble());
        std::string info;
        for(size_t i = 0 ; i < in.size(); ++i){
            info+=in[i];
        }

        std::shared_ptr<StringProtocol> msg = std::make_shared<StringProtocol>();
        msg->info = info;
        msg->setReqID("1111");
        out_messages.push_back(msg);

    } 

    ~StringCoder() {}


private:



};





}