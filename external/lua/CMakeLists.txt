 
cmake_minimum_required(VERSION 3.17)

project(Lua VERSION 5.4.4 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

set (lua_HEADERS
	src/lapi.h
	src/lauxlib.h
	src/lcode.h
	src/lctype.h
	src/ldebug.h
	src/ldo.h
	src/lfunc.h
	src/lgc.h
	src/ljumptab.h
	src/llex.h
	src/llimits.h
	src/lmem.h
	src/lobject.h
	src/lopcodes.h
	src/lopnames.h
	src/lparser.h
	src/lprefix.h
	src/lstate.h
	src/lstring.h
	src/ltable.h
	src/ltm.h
	src/luaconf.h
	src/lua.h
	src/lualib.h
	src/lundump.h
	src/lvm.h
	src/lzio.h
)

set (lua_SOURCES
	src/lapi.c
	src/lauxlib.c
	src/lbaselib.c
	src/lcode.c
	src/lcorolib.c
	src/lctype.c
	src/ldblib.c
	src/ldebug.c
	src/ldo.c
	src/ldump.c
	src/lfunc.c
	src/lgc.c
	src/linit.c
	src/liolib.c
	src/llex.c
	src/lmathlib.c
	src/lmem.c
	src/loadlib.c
	src/lobject.c
	src/lopcodes.c
	src/loslib.c
	src/lparser.c
	src/lstate.c
	src/lstring.c
	src/lstrlib.c
	src/ltable.c
	src/ltablib.c
	src/ltm.c
#	src/lua.c
#	src/luac.c
	src/lundump.c
	src/lutf8lib.c
	src/lvm.c
	src/lzio.c
)
set_source_files_properties(${lua_SOURCES} PROPERTIES LANGUAGE C)

add_library(lua STATIC ${lua_SOURCES} ${lua_HEADERS})
target_compile_definitions(lua PRIVATE LUA_COMPAT_5_3)
if (UNIX)
	target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
	target_link_options(lua PRIVATE "-Wl,-E")
endif()

target_link_libraries(lua PUBLIC m)
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(DEBUG "Lua: Linux detected")
	target_link_libraries(lua PUBLIC dl)
endif()

#[[ Uncomment to build interpreter

set_source_files_properties(src/lua.c PROPERTIES LANGUAGE C)
add_executable(luai ${lua_HEADERS} src/lua.c)
if (UNIX)
	target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
endif()
add_dependencies(luai lua)
target_link_libraries(luai PRIVATE lua)

set_source_files_properties(src/luac.c PROPERTIES LANGUAGE C)
add_executable(luac src/luac.c ${lua_HEADERS})
if (UNIX)
	target_compile_definitions(luac PRIVATE LUA_USE_LINUX)
endif()
add_dependencies(luac lua)
target_link_libraries(luac PRIVATE lua)

]]
