cmake_minimum_required(VERSION 3.9)
project(ipc)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-g")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)

LINK_DIRECTORIES(/usr/local/lib)
LINK_DIRECTORIES(/usr/lib64)


find_package(gflags COMPONENTS nothreads_static)
#find_package(glog COMPONENTS nothreads_static)


add_executable(ipcv
        src/ipc_comm.h
        src/ipc_server.cc)


add_executable(ipcc
        src/ipc_comm.h
        src/ipc_client.cc)

TARGET_LINK_LIBRARIES(ipcv rt gflags)
TARGET_LINK_LIBRARIES(ipcc rt)