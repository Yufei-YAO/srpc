#include "protocol/tiny_pb_protocol.h"

#include <sstream>
#include <iostream>
#include <iomanip>
namespace srpc{









int32_t TinyPBProtocol::getPBDataSize(int pack_len){
    return pack_len - (1 +4 +4+4+4+4 +4+1 +m_errInfo.size()+m_reqID.size()+m_methodName.size()); 
}




int32_t TinyPBProtocol::getPackageLen(){
    return  1 +4 +4+4+4+4 +4+1 +m_errInfo.size()+m_reqID.size()+m_methodName.size()+m_pbData.size();

}

std::string TinyPBProtocol::debugPBToHex(){
    std::string str = m_pbData;
    std::stringstream ss;
    for(size_t i = 0; i < str.size(); ++i) {
        if(i > 0 && i % 32 == 0) {
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0') << std::hex
           << (int)(uint8_t)str[i] << " ";
    }

    return ss.str();

}


}