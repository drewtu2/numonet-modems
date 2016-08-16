cmake_minimum_required(VERSION 2.8.9)
if (${CMAKE_VERSION} VERSION_GREATER 3.5)
  #  cmake_policy(SET CMP0046 OLD)
endif()

set(PROJECT_NAME Numonet-modem)
project(Numonet-modem)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
find_package(Threads REQUIRED)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_MACOSX_RPATH 1)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall -ansi -Wno-deprecated -pthread -std=c++11)
endif()

add_definitions(-std=c++11)
if("${CMAKE_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-std=c++11)
endif()

set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR})
include_directories(${COMMON_INCLUDES}) 

# Set source files
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} src/*.cpp)
message(STATUS "Source files: " ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

