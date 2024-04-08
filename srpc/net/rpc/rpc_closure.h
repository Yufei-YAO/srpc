#pragma once


#include <google/protobuf/service.h>

#include <functional>
namespace srpc{


class RpcClosure : public google::protobuf::Closure{
public:
typedef std::shared_ptr<RpcClosure> ptr;

    RpcClosure(std::function<void()> cb):m_cb(cb) {}
    ~RpcClosure(){

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


