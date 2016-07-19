file(GLOB h_files *.h)
file(GLOB cpp_files *.cpp)

add_library(fast
    STATIC ${cpp_files})

pods_install_headers(${h_files}
    DESTINATION fast)
pods_install_libraries(fast)

if(MAKE_TEST)
    add_executable(test_fast test.cpp)
    target_link_libraries(test_fast
        fast)
    pods_install_executables(test_fast)
endif()

