#pragma once
#include <memory>

namespace srpc{


struct AbstractProtocol {
    typedef std::shared_ptr<AbstractProtocol> ptr;


    std::string getReqID() { return m_reqID; }
    void setReqID(const std::string& id) { m_reqID = id; }

    virtual ~AbstractProtocol() {}



    std::string m_reqID;



};



}