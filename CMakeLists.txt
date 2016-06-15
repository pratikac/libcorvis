cmake_minimum_required(VERSION 2.8.6)

set(POD_NAME nucorvis)

if (POLICY CMP0054)
  cmake_policy(SET CMP0054 OLD)  # so variables inside quotes are still evaluated in if() statements
endif()

# set up Java, use a target of 1.6
find_package(Java REQUIRED)
include(UseJava)
set(CMAKE_JAVA_COMPILE_FLAGS ${CMAKE_JAVA_COMPILE_FLAGS} -source 6 -target 6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wreturn-type -Wuninitialized -Wunused-variable")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# pods.cmake has many convenience macros for
# installing executables, libraries etc.
include(cmake/pods.cmake)

# build lcm types
include(cmake/lcmtypes.cmake)
if(LCM_FOUND)
    lcmtypes_build()
else()
    message(FATAL_ERROR "[ERROR] LCM not found while building lcmtypes, cmake will exit.") 
endif()

# install nucore-spy
include(cmake/spy.cmake)
install_nucore_spy()

add_subdirectory(src)
