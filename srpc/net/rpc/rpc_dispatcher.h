#pragma once


#include <memory>
#include <map>
#include <google/protobuf/service.h>
#include "protocol/abstract_protocol.h"
#include "net/tcp/tcp_connection.h"
#include "protocol/tiny_pb_protocol.h"
#include "net/rpc/rpc_closure.h"
namespace srpc{







class RpcDispatcher {
public:
    typedef std::shared_ptr<RpcDispatcher> ptr;
    typedef std::shared_ptr<google::protobuf::Service> service_ptr;

    static RpcDispatcher::ptr GetRpcDispatcher();

    void dispatch(AbstractProtocol::ptr req, AbstractProtocol::ptr rsp, TcpConnection::ptr conn);

    void registerService(service_ptr s);
    void setTinyPBError(TinyPBProtocol::ptr msg, int32_t err_code, std::string err_info);

private:
    bool parseServiceName(const std::string & full_name, std::string& service_name, std::string& method_name);



private:
    std::map<std::string,service_ptr> m_services;









    
};








}