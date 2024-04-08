#include <string.h>
#include "common/log.h"
#include "protocol/tiny_pb_coder.h"
#include "protocol/tiny_pb_protocol.h"
#include "common/bytearray.h"
#include "common/endian.h"
#include "common/util.h"
namespace srpc{


//|START|PACKAGE_LEN|MSG_ID_LEN|MSG_ID|METHOD_NAME_LEN|METHOD_NAME|ERRCODE|ERR_INFO_LEN|ERR_INFO|PB_DATA|CRC|END|
//  8      32           32                     32                                                              
void TinyPBCoder::encode(std::vector<AbstractProtocol::ptr>& in_messages,TcpBuffer::ptr buffer) {
    for(auto &p : in_messages){
        TinyPBProtocol::ptr pbp = std::dynamic_pointer_cast<TinyPBProtocol>(p);
        //pbp->setPackageLen();
        if(pbp == nullptr){
            ERRORLOG("TinyPBCode encode fail");
            continue;
        }

        INFOLOG("size=%d, packageLen=%d start%d",array.getReadSize(),pbp->getPackageLen(),int8_t(TinyPBProtocol::Dlim::PB_START));
        array.writeFint8(int8_t(TinyPBProtocol::Dlim::PB_START));
        array.writeFint32(pbp->getPackageLen());
        array.writeStringF32(pbp->m_reqID);
        array.writeStringF32(pbp->m_methodName);
        array.writeFint32(pbp->m_errCode);
        array.writeStringF32(pbp->m_errInfo);
        array.writeStringWithoutLength(pbp->m_pbData);
        array.writeFint32(0);
        array.writeFint8(int8_t(TinyPBProtocol::Dlim::PB_END));
        if(array.getReadSize()>(size_t)buffer->writeAble()){
            //buffer->enlargeBuffer(array.getReadSize());
            buffer->resizeBuffer(1.5*(buffer->writeAble()+ array.getReadSize()));
        }
        int size = array.getReadSize();
        INFOLOG("array %s",array.toHexString().c_str());
        INFOLOG("%s",array.toHexString().c_str());
        array.read(&(buffer->m_cache[buffer->m_writeIndex]),size);
        array.clear();
        int32_t crc = computeCRC(&buffer->m_cache[buffer->m_writeIndex],size);
        *(int32_t*)&buffer->m_cache[buffer->m_writeIndex + size -5] = byteswapOnLittleEndian(crc);
        DEBUGLOG("read array to buffer %d",size);
        buffer->moveWriteIndex(size);
    }
}
void TinyPBCoder::decode(std::vector<AbstractProtocol::ptr>& out_messages,TcpBuffer::ptr buffer){
    int start_index = buffer->readIndex();
    int end_index = start_index;

    INFOLOG("packageLen=%d start_index=%d start uint=%d",buffer->readAble(),start_index,buffer->m_cache[start_index]);
    for(int i =start_index; i<buffer->writeIndex();++i){
        //INFOLOG("start_detect=%d",(int8_t)buffer->m_cache[i]);
        if(buffer->m_cache[i] == (int8_t)TinyPBProtocol::Dlim::PB_START){
            INFOLOG("start_detect=%d",i);
            if(i+4 >= buffer->writeIndex()){
                goto cleanBuffer; 
            }
            int32_t packageLen = byteswapOnLittleEndian(*(int32_t*)(&(buffer->m_cache[i+1])));
            INFOLOG("packlen=%d",packageLen);
            if(i + packageLen > buffer->writeIndex()){
                goto cleanBuffer;
            }
            INFOLOG("packageLen=%d",packageLen);
            int32_t crc_code = byteswapOnLittleEndian(*(int32_t*)(&buffer->m_cache[i+packageLen-5]));
            memset(&buffer->m_cache[i+packageLen-5],0,sizeof(int32_t));
            if(computeCRC(&buffer->m_cache[i],packageLen)!=crc_code){
                ERRORLOG("crc check failed");
                continue;
            }

            TinyPBProtocol::ptr p = std::make_shared<TinyPBProtocol>();
            //p->m_packageLen = packageLen;
            array.write(&buffer->m_cache[i],packageLen);
            INFOLOG("array readAble=%d",array.getReadSize());
            array.readFint8();
            array.readFint32();
            p->m_reqID = array.readStringF32();
            p->m_methodName = array.readStringF32();
            p->m_errCode = array.readFint32();
            p->m_errInfo = array.readStringF32();
            p->m_pbData.resize(packageLen-p->getPackageLen());
            INFOLOG("array readAble=%d size=%d",array.getReadSize(),p->m_pbData.size());
            array.read(&(p->m_pbData[0]),p->m_pbData.size());
            array.readFint32();

            INFOLOG("array %s",array.toHexString().c_str());
            p->m_checkSum = crc_code;
            int8_t end_dlim = array.readFint8();
            if(end_dlim != (int8_t)TinyPBProtocol::Dlim::PB_END){
                ERRORLOG("TinyPBCode decode wrong with array not stop at PB_END");
                return;
            }
            if(array.getReadSize()!=0){
                ERRORLOG("TinyPBCode decode wrong with array not empty after decoding");
                return;
            }
            INFOLOG("decode succes=%d",packageLen);
            out_messages.push_back(p); 
            end_index = i+p->getPackageLen();
            i = end_index -1;
            continue;
        }
    }
cleanBuffer:
    buffer->moveReadIndex(end_index-start_index);
}

    



}