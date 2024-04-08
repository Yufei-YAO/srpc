#pragma once
#include "protocol/abstract_protocol.h"


namespace srpc{


struct TinyPBProtocol : public AbstractProtocol{
public:
    typedef std::shared_ptr<TinyPBProtocol> ptr;
    enum class Dlim : int8_t{
        PB_START = 1,
        PB_END = 2
    };

    int32_t getPBDataSize(int pack_len);

    int32_t getPackageLen();

    std::string debugPBToHex();

//|START|PACKAGE_LEN|MSG_ID_LEN|MSG_ID|METHOD_NAME_LEN|METHOD_NAME|ERRCODE|ERR_INFO_LEN|ERR_INFO|PB_DATA|CRC|END|
//  1      4           4                     4                        4       4                      4       1
    //int32_t m_packageLen;
    // std::string m_reqID;
    std::string m_methodName;
    int32_t m_errCode;
    std::string m_errInfo;
    std::string m_pbData;
    int32_t m_checkSum;

    //bool m_paraseSuccess={false};


};










}