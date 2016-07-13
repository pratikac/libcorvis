cmake_minimum_required(VERSION 3.1)
set(POD_NAME vikit)

if(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    find_file(_build_dir build PATHS ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/.. ${PROJECT_SOURCE_DIR}/../../)
    if(_build_dir)
        set(CMAKE_INSTALL_PREFIX "${_build_dir}" CACHE PATH "Install location for project" FORCE)
    endif()
endif()
message(STATUS CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wreturn-type")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -msse -msse -msse2 -msse3 -mssse3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops")

add_subdirectory(src)
add_subdirectory(test)
