#include "net/rpc/rpc_channel.h"
#include "protocol/tiny_pb_protocol.h"
#include "net/rpc/rpc_controller.h"
#include "net/rpc/rpc_closure.h"
#include "net/tcp/tcp_client.h"
#include "common/log.h"
#include "protocol/req_id.h"
#include "net/net_addr.h"
#include "net/rpc/rpc_error_code.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


namespace srpc{

RpcChannel::RpcChannel(NetAddr::ptr peer){
    INFOLOG("RpcChannel");
    m_peerAddr = peer;
}
RpcChannel::~RpcChannel(){
    INFOLOG("~RpcChannel");
}

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done){
    
    TinyPBProtocol::ptr req_msg = std::make_shared<srpc::TinyPBProtocol>();
    //s->setReqID("1111");

    
    RpcController* control = dynamic_cast<RpcController*>(controller);
    if(control == nullptr || request == nullptr || response == nullptr || method == nullptr){
        ERRORLOG("some thing wrong %p %p %p %p",control,request,response,method);
        return;
    }
    if(control->GetReqID().empty()){
        control->SetReqID(ReqIDUtil::GenReqID());
    }
    req_msg->setReqID(control->GetReqID());
    req_msg->m_methodName = method->full_name();

    if(!request->SerializeToString(&req_msg->m_pbData)){
        control->SetErrorCode(ERROR_FAILED_SERIALIZE,"failed serialize request");
        return;

    }

    TcpClient::ptr cli =  std::make_shared<TcpClient>(m_peerAddr);
    cli->connect([cli,req_msg,response,done](){
        cli->writeMessage(req_msg,
            [cli,req_msg,response,done](AbstractProtocol::ptr p){
                cli->readMessage(req_msg->getReqID(),[cli,response,done](AbstractProtocol::ptr p){
                    TinyPBProtocol::ptr rsp = std::dynamic_pointer_cast<TinyPBProtocol>(p);
                    if(rsp->m_errCode){
                        ERRORLOG("rpc read fail with m_errCode=%d m_errinfo=%s",rsp->m_errCode,rsp->m_errInfo.c_str());
                        return;
                    }
                    if(!response->ParseFromString(rsp->m_pbData)){
                        ERRORLOG("rpc read fail with respone decode error=%s",rsp->debugPBToHex().c_str());
                        return;
                    }
                    if(done){
                        done->Run();
                    } 
                    cli->stop();
                    
                });
            }
        );
    });
                            
}


}