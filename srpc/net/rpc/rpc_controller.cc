#include "net/rpc/rpc_controller.h"


namespace srpc{





void RpcController::Reset(){
    m_error_code = 0 ;
    m_error_info = "";
    m_req_id = "";

    m_is_failed   = {false};
    m_is_cancled  = {false};
    m_is_finished = {false};

    m_local_addr = nullptr;
    m_peer_addr = nullptr;

    m_timeout = {1000};   // ms
}


bool RpcController::Failed() const{
    return m_is_failed;
}

std::string RpcController::ErrorText() const{
    return m_error_info;
}

void RpcController::StartCancel(){
    m_is_cancled = true;
}


void RpcController::SetFailed(const std::string& reason){
    m_is_failed = true;
    m_error_info = reason;
}

bool RpcController::IsCanceled() const{
    return m_is_cancled;
}

void RpcController::NotifyOnCancel(google::protobuf::Closure* callback){

}





}