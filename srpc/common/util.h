#include <sys/syscall.h>
#include <sys/types.h>


namespace srpc
{
int32_t getProcessID() ;
int32_t getThreadID() ;


uint64_t getCurrentMS();
} // namespace srpc
