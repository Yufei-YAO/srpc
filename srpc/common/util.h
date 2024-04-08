#include <sys/syscall.h>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <iostream>
#include <iomanip>
namespace srpc
{
int32_t getProcessID() ;
int32_t getThreadID() ;


uint64_t getCurrentMS();

int32_t computeCRC(char * buf, size_t buf_size);

std::string toHexString(char* tmp, int size);


} // namespace srpc
