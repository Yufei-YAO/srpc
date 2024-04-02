#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <cstdint>
#include <sys/time.h>
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
}