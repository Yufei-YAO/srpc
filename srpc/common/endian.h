#pragma once
#include <byteswap.h>
#include <stdint.h>
#include <type_traits>

#define SRPC_LITTLE_ENDIAN 1
#define SRPC_BIG_ENDIAN 2
namespace srpc
{
template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value){
    return (T)bswap_64((uint64_t)value);
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value){
    return (T)bswap_32((uint32_t)value);
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value){
    return (T)bswap_16((uint16_t)value);
}


#if BYTE_ORDER == BIG_ENDIAN
#define SRPC_BYTE_ORDER SRPC_BIG_ENDIAN
#else
#define SRPC_BYTE_ORDER SRPC_LITTLE_ENDIAN
#endif

#if SRPC_BYTE_ORDER == SRPC_BIG_ENDIAN
template<typename T>
T byteswapOnLittleEndian(T t){
    return t;
}

template<typename T>
T byteswapOnBigEndian(T t){
    return byteswap(t);
}

#else
template<typename T>
T byteswapOnLittleEndian(T t){
    return byteswap(t);
}

template<typename T>
T byteswapOnBigEndian(T t){
    return t;
}



#endif



} // namespace ssever
