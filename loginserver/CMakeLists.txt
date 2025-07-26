# 最低版本要求，2.8即可
cmake_minimum_required(VERSION 2.8)

project(loginserver)

add_definitions(-std=c++11)
add_definitions(-fpermissive)


# 项目自身头文件
include_directories(.)
include_directories(..)
include_directories(../servercommon)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../3rdpart/mysqlconnector/v32/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common/platform/thread)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include/common/md5)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../servercommon)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../servercommon/multithread)

# 第三方库目录
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/lib_release)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/engine_lib_release)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/protobuf)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/tinyxml2)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../3rdpart/mysqlconnector/v64_linux/lib)

add_definitions(-fpermissive)

# 查找源文件
file(GLOB_RECURSE SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE SERVERCOMMON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../servercommon/*.cpp")
file(GLOB_RECURSE PROTOBUF_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../servercommon/protobuf/*.cc")

# 手动排除 build 目录（CMake 2.8不支持 list(FILTER...)，只能循环删除）
set(EXCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(FILTERED_SRC_FILES "")
foreach(src_file ${SRC_FILES})
    string(FIND "${src_file}" "${EXCLUDE_DIR}" EXCLUDE_FOUND)
    if(EXCLUDE_FOUND EQUAL -1)
        list(APPEND FILTERED_SRC_FILES "${src_file}")
    endif()
endforeach()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(TARGET_NAME EXEloginserver_)
# CMAKE_POSITION_INDEPENDENT_CODE 在旧版 cmake 可注释掉
# set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

add_executable(${TARGET_NAME}
    ${FILTERED_SRC_FILES}
    ${SERVERCOMMON_FILES}
    ${PROTOBUF_FILES}
    # 如有其它单独 cpp 文件可加在这里
)

# 链接依赖库
target_link_libraries(${TARGET_NAME}
    pthread
    rt
    mysql
    mysqlclient
    dl
    hiredis
    protobuf
    tinyxml2
    config
    network
    log
    gameserver
    utility
    rmi
    basicnetwork
    common
)