cmake_minimum_required(VERSION 3.1)

set(POD_NAME libcorvis)

if(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    find_file(_build_dir build PATHS ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/.. ${PROJECT_SOURCE_DIR}/../../)
    if(_build_dir)
        set(CMAKE_INSTALL_PREFIX "${_build_dir}" CACHE PATH "Install location for project" FORCE)
    endif()
endif()
message(STATUS CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

# set up Java, use a target of 1.6
find_package(Java REQUIRED)
include(UseJava)
set(CMAKE_JAVA_COMPILE_FLAGS ${CMAKE_JAVA_COMPILE_FLAGS} -source 6 -target 6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wreturn-type")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -msse -msse -msse2 -msse3 -mssse3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops")

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

# install spy
include(cmake/spy.cmake)
install_corvis_spy()

# find vision libraries
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)

add_subdirectory(src)
add_subdirectory(externals)
