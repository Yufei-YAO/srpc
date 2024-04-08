#pragma once


#include <google/protobuf/service.h>
#include <memory>
#include <functional>
#include "common/log.h"
namespace srpc{


class RpcClosure : public google::protobuf::Closure{
public:
typedef std::shared_ptr<RpcClosure> ptr;

    RpcClosure(std::function<void()> cb):m_cb(cb) {}
    ~RpcClosure(){
        DEBUGLOG("~RpcClosure");
    }

    void Run(){
        if(m_cb){
            m_cb();
        }
    }
private:
    std::function<void()> m_cb;


};








}


