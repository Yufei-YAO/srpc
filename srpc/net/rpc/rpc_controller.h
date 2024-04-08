#pragma once


#include <google/protobuf/service.h>
#include <memory>
#include "net/net_addr.h"


namespace srpc{


class RpcController : public google::protobuf::RpcController{
public:
    typedef std::shared_ptr<RpcController> ptr;

    ~RpcController() {}



    void Reset();


    bool Failed() const;

    std::string ErrorText() const;

    void StartCancel() ;

  // Server-side methods ---------------------------------------------

    void SetFailed(const std::string& reason);

    bool IsCanceled() const;

    void NotifyOnCancel(google::protobuf::Closure* callback);



    void SetTimeout(int t){
        m_timeout = t;
    }
    int GetTimeout(){
        return m_timeout;
    }

    void SetLocalAddr(NetAddr::ptr addr){
        m_local_addr = addr;
    }
    void SetPeerAddr(NetAddr::ptr addr){
        m_peer_addr = addr;
    }
    NetAddr::ptr GetLocalAddr(){
        return m_local_addr;
    }
    NetAddr::ptr GetPeerAddr(){
        return m_peer_addr;
    }

    void SetErrorCode(int32_t e, const std::string& e_info){
        m_is_failed = true;
        m_error_code = e;
        m_error_info = e_info;
    }
    int32_t GetErrorCode(){
        return m_error_code;
    }
    std::string GetErrorInfo(){
        return m_error_info;
    }

    void SetReqID(const std::string& id){
        m_req_id = id;
    }
    std::string GetReqID(){
        return m_req_id;
    }




private:

    int32_t m_error_code {0};
    std::string m_error_info;
    std::string m_req_id;

    bool m_is_failed {false};
    bool m_is_cancled {false};
    bool m_is_finished {false};

    NetAddr::ptr m_local_addr;
    NetAddr::ptr m_peer_addr;

    int m_timeout {1000};   // ms



};




}


