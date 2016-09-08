cmake_minimum_required(VERSION 2.6)

PROJECT(xspnet)

set(CMAKE_BUILD_TYPE "Debug")

set(CXX_FLAGS
  -g
  -O0
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 #-std=c++0x
 -rdynamic
 )

set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 ")
set(CMAKE_CXX_FLAGS_RELEASE "-fPIC -W -O2 -finline-limit=1000 -DNDEBUG")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(examples/TcpService)