cmake_minimum_required(VERSION 2.8)
project(srpc)


set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -std=c++17 -g -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)


find_library(YAMLCPP libyaml-cpp.a)
include_directories(${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/srpc)

file(GLOB_RECURSE LIB_SRC srpc/*.cc)
add_library(srpc SHARED ${LIB_SRC})
set(LIB_LIB srpc pthread ${YAMLCPP})


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