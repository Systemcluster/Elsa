cmake_minimum_required(VERSION 2.8)
project(Elsa)

set(LUA_INCLUDE_DIR "/usr/local/include" CACHE FILEPATH "Directory containing lua.hpp")
set(LUA_LIBRARY_DIR "/usr/local/lib" CACHE FILEPATH "Directory containing lua library")

if(${LUA_INCLUDE_DIR})
    if(NOT EXISTS ${LUA_INCLUDE_DIR}/lua.hpp)
        message(SEND_ERROR "Could not find lua.hpp in ${LUA_INCLUDE_DIR}")
    endif()
endif()

include_directories(${LUA_INCLUDE_DIR})
link_directories(${LUA_LIBRARY_DIR})

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -stdlib=libc++")
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")
endif(APPLE)

include_directories(include)
file(GLOB headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
include/*.hpp include/elsa/*.hpp)

add_executable(elsa_test ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)
target_link_libraries(elsa_test lua)
