cmake_minimum_required(VERSION 2.8)
project(srpc)


set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -std=c++17 -g -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)


find_library(YAMLCPP libyaml-cpp.a)
#include_directories(${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/srpc)

file(GLOB_RECURSE LIB_SRC srpc/*.cc)
add_library(srpc SHARED ${LIB_SRC})
set(LIB_LIB srpc pthread ${YAMLCPP} protobuf)


add_executable(test_hello test/test_hello.cc)
add_dependencies(test_hello srpc)
target_link_libraries(test_hello ${LIB_LIB})


add_executable(test_log test/test_log.cc)
add_dependencies(test_log srpc)
target_link_libraries(test_log ${LIB_LIB})

add_executable(test_config test/test_config.cc)
add_dependencies(test_config srpc)
target_link_libraries(test_config ${LIB_LIB})

add_executable(test_eventLoop test/test_eventLoop.cc)
add_dependencies(test_eventLoop srpc)
target_link_libraries(test_eventLoop ${LIB_LIB})

add_executable(test_buffer test/test_buffer.cc)
add_dependencies(test_buffer srpc)
target_link_libraries(test_buffer ${LIB_LIB})

add_executable(test_tcpserver test/test_tcpserver.cc)
add_dependencies(test_tcpserver srpc)
target_link_libraries(test_tcpserver ${LIB_LIB})

add_executable(test_connection test/test_connection.cc)
add_dependencies(test_connection srpc)
target_link_libraries(test_connection ${LIB_LIB})

add_executable(test_tcpclient test/test_tcpclient.cc)
add_dependencies(test_tcpclient srpc)
target_link_libraries(test_tcpclient ${LIB_LIB})

add_executable(test_tinyPB test/test_tinyPB.cc)
add_dependencies(test_tinyPB srpc)
target_link_libraries(test_tinyPB ${LIB_LIB})

add_executable(test_endian test/test_endian.cc)

add_executable(test_rpcclient test/test_rpcclient.cc test/order.pb.cc)
add_dependencies(test_rpcclient srpc)
target_link_libraries(test_rpcclient ${LIB_LIB})

add_executable(test_rpcserver test/test_rpcserver.cc test/order.pb.cc)
add_dependencies(test_rpcserver srpc)
target_link_libraries(test_rpcserver ${LIB_LIB})