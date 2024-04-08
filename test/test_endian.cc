#include <iostream>
#include "../srpc/common/endian.h"
#include <unistd.h>
int main(){
    uint64_t a = 0x1;
    uint64_t b = srpc::byteswapOnLittleEndian(a);
    std::cout<<b<<std::endl;
    std::cout<<srpc::byteswapOnBigEndian(a)<<std::endl;
    return 0;

}