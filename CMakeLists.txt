cmake_minimum_required(VERSION 3.9)
project(ipc)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-g")


#find_package(gflags COMPONENTS nothreads_static)
#find_package(glog COMPONENTS nothreads_static)

LINK_DIRECTORIES(/usr/local/lib)
LINK_DIRECTORIES(/usr/lib64)

add_executable(ipcv
        ipc_comm.h
        ipc_server.cc)


add_executable(ipcc
        ipc_comm.h
        ipc_client.cc)

TARGET_LINK_LIBRARIES(ipcv rt)
TARGET_LINK_LIBRARIES(ipcc rt)