#pragma once

#include <vector>
#include <memory>
class TcpConnection;
namespace srpc
{
class TcpBuffer{
public:
friend class TcpConnection;

    typedef std::shared_ptr<TcpBuffer> ptr;

    TcpBuffer(int size) ;
    ~TcpBuffer();


    int readAble() { return m_writeIndex - m_readIndex;}
    int writeAble() { return m_size - m_writeIndex;}

    int readIndex() {return m_readIndex;}
    int writeIndex() {return m_writeIndex;}

    void writeToBuffer(const char* buf, int size);
    void readFromBuffer(std::vector<char>& buf, int size);

    void moveWriteIndex(int size);
    void moveReadIndex(int size);

    void resizeBuffer(int size);
private:
    void adjustBuffer();


private:
    int m_readIndex = 0;
    int m_writeIndex = 0;
    int m_size = 0;
    std::vector<char> m_cache;



};
    
} // namespace srpc
