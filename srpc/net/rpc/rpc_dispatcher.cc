#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "net/rpc/rpc_controller.h"
#include "net/rpc/rpc_dispatcher.h"
#include "net/rpc/rpc_error_code.h"
#include "net/rpc/rpc_closure.h"
#include "common/log.h"
namespace srpc{


static RpcDispatcher::ptr g_dispatcher = nullptr;


RpcDispatcher::ptr RpcDispatcher::GetRpcDispatcher(){
    if(g_dispatcher != nullptr){
        return g_dispatcher;
    }
    g_dispatcher = std::make_shared<RpcDispatcher>();
    return g_dispatcher;
}



void RpcDispatcher::dispatch(AbstractProtocol::ptr req_ptr, AbstractProtocol::ptr rsp_ptr, TcpConnection::ptr conn){
    TinyPBProtocol::ptr req = std::dynamic_pointer_cast<TinyPBProtocol>(req_ptr);
    TinyPBProtocol::ptr rsp = std::dynamic_pointer_cast<TinyPBProtocol>(rsp_ptr);
    if(req == nullptr || rsp == nullptr){
        ERRORLOG("dispatch fails with nullptr req=%d rsp=%d",req,rsp);
        throw std::runtime_error("dispatch fails with nullptr");
    }

    std::string full_name = req->m_methodName;
    std::string service_name;
    std::string method_name;
    if(!parseServiceName(full_name,service_name,method_name)){
        ERRORLOG("parse service name error full_name=%s",full_name.c_str());
        setTinyPBError(rsp,ERROR_PARSE_SERVICE_NAME,"parse service name fail");
        return;
    }
    rsp->m_methodName = full_name;
    rsp->m_reqID = req->m_reqID;

    auto ser = m_services.find(service_name);
    if(ser == m_services.end()){
        ERRORLOG("service not found=%s",full_name.c_str());
        setTinyPBError(rsp,ERROR_SERVICE_NOT_FOUND,"service not found");
        return;
    }
    auto service = ser->second;
    auto met = service->GetDescriptor()->FindMethodByName(method_name);
    if(met == nullptr){
        ERRORLOG("method not found=%s",full_name.c_str());
        setTinyPBError(rsp,ERROR_METHOD_NOT_FOUND,"method not found");
        //return;
        return;
    } 

    std::shared_ptr<google::protobuf::Message> req_msg(service->GetRequestPrototype(met).New());
    if(!req_msg->ParseFromString(req->m_pbData)){
        ERRORLOG("parse req not valid=%s",req->debugPBToHex().c_str());
        setTinyPBError(rsp,ERROR_METHOD_NOT_FOUND,"parse req not valid");
        //return;
        return;
    }
    INFOLOG("%s | get rpc request[%s]", req->m_reqID.c_str(), req_msg->ShortDebugString().c_str());
    std::shared_ptr<google::protobuf::Message> rsp_msg(service->GetResponsePrototype(met).New());
    RpcController::ptr controller = std::make_shared<RpcController>();
    controller->SetLocalAddr(conn->getLocalAddr());
    controller->SetPeerAddr(conn->getPeerAddr());
    controller->SetReqID(req->m_reqID);
    RpcClosure::ptr closu = std::make_shared<RpcClosure>(
        [rsp,rsp_msg](){
            //exit(0);
            INFOLOG("call closu sucee");
            if(!rsp_msg->SerializePartialToString(&rsp->m_pbData)){
                ERRORLOG("rsp_msg serial falut");
                RpcDispatcher::GetRpcDispatcher()->setTinyPBError(rsp,ERROR_FAILED_SERIALIZE,"failed to deserialize rsp");
                return;
            }
            rsp->m_errCode = 0;
            rsp->m_errInfo = "";
            INFOLOG("call closu sucee");
            //closu.reset();
        }
    );
    

    service->CallMethod(met, controller.get(), req_msg.get(), rsp_msg.get(), closu.get());
//const google::protobuf::MethodDescriptor *method, google::protobuf::RpcController *controller, 
//const google::protobuf::Message *request, google::protobuf::Message *response, google::protobuf::Closure *done
    INFOLOG("%s | call rpc success [%s]", rsp->m_reqID.c_str(), rsp_msg->ShortDebugString().c_str());
    //sleep(10);

}

void RpcDispatcher::registerService(service_ptr s){
    std::string service_name = s->GetDescriptor()->full_name();
    m_services[service_name]=s;

}
void RpcDispatcher::setTinyPBError(TinyPBProtocol::ptr msg, int32_t err_code, std::string err_info){
    msg->m_errCode = err_code;
    msg->m_errInfo = err_info;
}


bool RpcDispatcher::parseServiceName(const std::string & full_name, std::string& service_name, std::string& method_name){
    size_t i = full_name.find_first_of('.');
    if(i == full_name.npos){
        ERRORLOG("parseServiceName fail with full_name not contain .");
        return false;
    }
    service_name = full_name.substr(0,i);
    method_name = full_name.substr(i+1);

    return true;
}
















}