
INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(../servercommon)

FILE(GLOB_RECURSE SRC_FILES "*.cpp")
FILE(GLOB_RECURSE COMMON_FILES "../servercommon/*.cpp")
FILE(GLOB_RECURSE PROTOBUF_FILES "../servercommon/protobuf/*.cc")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
ADD_EXECUTABLE(EXEgateway_${ARPG_BUILD_TYPE} ${COMMON_FILES} ${SRC_FILES} ${PROTOBUF_FILES})

TARGET_LINK_LIBRARIES(EXEgateway_${ARPG_BUILD_TYPE} pthread rt libconfig.so
libnetwork.so librmi.so liblog.so libgameserver.a libutility.a
libbasicnetwork.a libcommon.a libmysql.so libmysqlclient.so libprotobuf.a libdl.so libhiredis.a)

