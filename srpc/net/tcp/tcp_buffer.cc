#include "net/tcp/tcp_buffer.h"
#include "common/log.h"
#include <cstring>
namespace srpc{



TcpBuffer::TcpBuffer(int size){
    m_size = size;
    m_cache.resize(m_size);
}
TcpBuffer::~TcpBuffer(){

}



void TcpBuffer::writeToBuffer(const char* buf, int size){
    if(size > writeAble()){
        int new_size = 1.5*(size + m_writeIndex);
        resizeBuffer(new_size);
    }
    memcpy(&m_cache[m_writeIndex],buf,size);
    m_writeIndex += size;

}
void TcpBuffer::readFromBuffer(std::vector<char>& buf, int size){
    if (readAble() == 0) {
      return;
    }

    int read_size = readAble() > size ? size : readAble();
    std::vector<char> tmp(read_size);
    memcpy(&tmp[0], &m_cache[m_readIndex], read_size);

    buf.swap(tmp); 
    m_readIndex += read_size;

    adjustBuffer();

}

void TcpBuffer::moveWriteIndex(int size){
    size_t cur = m_writeIndex +size;
    if(cur>m_cache.size()){
        ERRORLOG("move writeIndex out of range with buffer size=%d m_writeIndex=%d size=%d",m_size,m_readIndex,size);
        return;
    }
    m_writeIndex = cur;
    // if(m_writeIndex == m_size){
    //     resizeBuffer(2*m_size);
    // }
}
void TcpBuffer::moveReadIndex(int size){
    size_t cur = m_readIndex +size;
    if(cur>m_cache.size()){
        ERRORLOG("move readIndex out of range with buffer size=%d m_readIndex=%d size=%d",m_size,m_readIndex,size);
        return;
    }
    m_readIndex = cur;
    adjustBuffer();

}

void TcpBuffer::enlargeBuffer(int size){
    size = m_writeIndex += size;
    if(size < m_size){
        return;
    }
    m_cache.resize(1.5*size);
    m_size = m_cache.size();

}
void TcpBuffer::resizeBuffer(int size){
    if(size < readAble()){
        ERRORLOG("resize buffer fail with new size=%d smaller than current readAble=%d with current size=%d",size,readAble(),m_size);
        return;
    }
    m_cache.resize(size);
    m_size = size;
}
void TcpBuffer::adjustBuffer(){
    if(m_readIndex < int(m_cache.size()/3)){
        return;
    }
    std::vector<char> tmp(m_cache); 
    memcpy(&m_cache[0],&tmp[0],readAble());
    m_writeIndex = readAble();
    m_readIndex = 0;
}






}