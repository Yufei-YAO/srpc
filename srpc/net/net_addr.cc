#include <strings.h>
#include "net/net_addr.h"


namespace srpc{



IPv4NetAddr::IPv4NetAddr(const sockaddr_in addr) {
    m_addr = addr;
    m_ip = std::string(inet_ntoa(m_addr.sin_addr));
    m_port = ntohs(m_addr.sin_port);
    m_valid_port = true;
}
IPv4NetAddr::IPv4NetAddr(const std::string& addr){
    bzero(&m_addr,sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr =  inet_addr(addr.c_str());    
    m_ip = addr;

}

IPv4NetAddr::IPv4NetAddr(const std::string& addr, uint16_t port):IPv4NetAddr(addr){
    setPort(port); 
}

std::string IPv4NetAddr::toString(){
    if(m_valid_port) 
        return m_ip + ":" + std::to_string(m_port);
    else
        return m_ip ;

}

void IPv4NetAddr::setPort(uint16_t p) {
    m_valid_port = true, 
    m_port = p;
    m_addr.sin_port = htons(p);
}
bool IPv4NetAddr::checkValid() {
    if (!m_valid_port) return false;
    if (inet_addr(m_ip.c_str()) == INADDR_NONE) {
        return false;
    }
    return true;


}

} 