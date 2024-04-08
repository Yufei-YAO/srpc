#pragma once



#ifndef SRPC_ERROR_PREFIX
#define SRPC_ERROR_PREFIX(xx) 1000##xx
#endif 

const int ERROR_PEER_CLOSED = SRPC_ERROR_PREFIX(0000);    // 连接时对端关闭
const int ERROR_FAILED_CONNECT = SRPC_ERROR_PREFIX(0001);  // 连接失败
const int ERROR_FAILED_GET_REPLY = SRPC_ERROR_PREFIX(0002);  // 获取回包失败
const int ERROR_FAILED_DESERIALIZE = SRPC_ERROR_PREFIX(0003);    // 反序列化失败
const int ERROR_FAILED_SERIALIZE = SRPC_ERROR_PREFIX(0004);      // 序列化 failed

const int ERROR_FAILED_ENCODE = SRPC_ERROR_PREFIX(0005);      // encode failed
const int ERROR_FAILED_DECODE = SRPC_ERROR_PREFIX(0006);      // decode failed

const int ERROR_RPC_CALL_TIMEOUT = SRPC_ERROR_PREFIX(0007);    // rpc 调用超时

const int ERROR_SERVICE_NOT_FOUND = SRPC_ERROR_PREFIX(0008);    // service 不存在
const int ERROR_METHOD_NOT_FOUND = SRPC_ERROR_PREFIX(0009);    // method 不存在 method 
const int ERROR_PARSE_SERVICE_NAME = SRPC_ERROR_PREFIX(0010);    // service name 解析失败
const int ERROR_RPC_CHANNEL_INIT = SRPC_ERROR_PREFIX(0011);    // rpc channel 初始化失败
const int ERROR_RPC_PEER_ADDR = SRPC_ERROR_PREFIX(0012);    // rpc 调用时候对端地址异常