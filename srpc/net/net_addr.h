#pragma once
#include <memory>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
namespace srpc
{

class NetAddr{
public:
    typedef std::shared_ptr<NetAddr> ptr;


    virtual sockaddr* getAddr() = 0;
    virtual socklen_t getAddrLen() = 0;

    virtual int getFamily() = 0;

    virtual std::string toString() = 0;

    virtual bool checkValid() = 0;
private:




};


class IPv4NetAddr : public NetAddr {
public:
    typedef std::shared_ptr<IPv4NetAddr> ptr;

    IPv4NetAddr(const sockaddr_in addr) ;
    IPv4NetAddr(const std::string& addr);
    IPv4NetAddr(const std::string& addr, uint16_t port);
    sockaddr* getAddr() override{return (sockaddr*)&m_addr;}
    socklen_t getAddrLen() override{return sizeof(m_addr);}

    int getFamily() override{ return m_addr.sin_family;}

    std::string toString() override;

    bool checkValid() override;

    uint16_t getPort() {return m_port;}
    void setPort(uint16_t p) ;

private:
    std::string m_ip;
    uint16_t m_port;
    
    sockaddr_in m_addr;
    bool m_valid_port = false;

};

} // namespace srpc
