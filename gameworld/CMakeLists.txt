
INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(./gameworld)
INCLUDE_DIRECTORIES(../servercommon)

FILE(GLOB_RECURSE SRC_FILES "*.cpp")
FILE(GLOB_RECURSE COMMON_FILES "../servercommon/*.cpp")
FILE(GLOB_RECURSE PROTOBUF_FILES "../servercommon/protobuf/*.cc")
FILE(GLOB_RECURSE LUA_FILES "../../3rdpart/lua5.1/*.c")
FILE(GLOB_RECURSE TOLUA_FILES "../../3rdpart/tolua++/src/*.c")


SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
ADD_EXECUTABLE(EXEgameworld_${ARPG_BUILD_TYPE} ${COMMON_FILES} ${SRC_FILES} ${LUA_FILES} ${TOLUA_FILES} ${PROTOBUF_FILES})

TARGET_LINK_LIBRARIES(EXEgameworld_${ARPG_BUILD_TYPE} pthread rt libconfig.so
libnetwork.so librmi.so liblog.so libgameserver.a libutility.a
libbasicnetwork.a libcommon.a libmysql.so libmysqlclient.so libprotobuf.a libdl.so libhiredis.a)

