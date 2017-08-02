cmake_minimum_required(VERSION 2.8)
project(Elsa)


set(LUA_INCLUDE_DIR CACHE FILEPATH "Directory containing lua.hpp")
set(LUA_LIBRARY_DIR CACHE FILEPATH "Directory containing lua library")

if(${LUA_INCLUDE_DIR})
    if(NOT EXISTS ${LUA_INCLUDE_DIR}/lua.hpp)
        message(SEND_ERROR "Could not find lua.hpp in ${LUA_INCLUDE_DIR}")
    endif()
endif()

# prefer the user specified directory and luajit directories to the default ones
include_directories(BEFORE SYSTEM ${LUA_INCLUDE_DIR} /opt/local/include/luajit /usr/local/include/luajit /opt/local/include/luajit-2.1 /usr/local/include/luajit-2.1 /opt/local/include/luajit-2.0 /usr/local/include/luajit-2.0)
link_directories(${LUA_LIBRARY_DIR})

# prefer libluajit to liblua
find_library(LUA_LIB NAMES luajit luajit-5.1 lua)


if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++1z")
    set(CMAKE_EXE_LINKER_FLAGS "-pagezero_size 10000 -image_base 100000000")
    add_definitions(-DCOLOURED_OUTPUT)
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++1z")
    add_definitions(-DCOLOURED_OUTPUT)
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++1z")
endif(APPLE)

add_definitions(-DDEBUG)


include_directories(include)
file(GLOB headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} include/*.hpp include/elsa/*.hpp)


add_executable(elsa_test ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)
target_link_libraries(elsa_test ${LUA_LIB})
