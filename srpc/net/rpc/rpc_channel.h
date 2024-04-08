#pragma once

#include "net/net_addr.h"
#include <google/protobuf/service.h>





namespace srpc{


class RpcChannel : public google::protobuf::RpcChannel{
public:
    RpcChannel(NetAddr::ptr peer);
    ~RpcChannel();

  // Call the given method of the remote service.  The signature of this
  // procedure looks the same as Service::CallMethod(), but the requirements
  // are less strict in one important way:  the request and response objects
  // need not be of any specific class as long as their descriptors are
  // method->input_type() and method->output_type().
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done);


private:
    NetAddr::ptr m_peerAddr;

};





}