file(GLOB h_files *.h)
file(GLOB cpp_files *.cpp)

add_library(vikit STATIC ${cpp_files})

pods_install_headers(${h_files}
    DESTINATION vikit)
pods_install_libraries(vikit)
