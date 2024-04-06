#pragma once
#include <memory>

namespace srpc{


class AbstractProtocol {
public:
    typedef std::shared_ptr<AbstractProtocol> ptr;


    std::string getReqID() { return m_reqID; }
    void setReqID(const std::string& id) { m_reqID = id; }

    virtual ~AbstractProtocol() {}



protected:
    std::string m_reqID;



};



}