#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <cstdint>
#include <sys/time.h>
#include <boost/crc.hpp>
#include "common/util.h"


namespace srpc{
static int g_pid = 0;
static thread_local int t_thread_id = 0;
int32_t getProcessID(){
    if(g_pid!=0){
        return g_pid;
    }
    g_pid = getpid();
    return g_pid ;
}
int32_t getThreadID(){
    if (t_thread_id != 0) {
        return t_thread_id;
    }
    t_thread_id = syscall(SYS_gettid);
    return t_thread_id;
}

uint64_t getCurrentMS(){
    struct timeval tv;
    gettimeofday(&tv,0);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}


int32_t computeCRC(char * buf, size_t buf_size){
    boost::crc_32_type crc_t; // 创建一个CRC对象
    crc_t.process_bytes(buf, buf_size); // 处理数据
    return (int32_t)crc_t.checksum();
}
std::string toHexString(char* tmp, int size) {
    std::string str(tmp,size);
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