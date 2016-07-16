file(GLOB h_files *.h)
file(GLOB hpp_files *.hpp)
file(GLOB cpp_files *.cpp)

add_library(vikit STATIC ${cpp_files})
target_link_libraries(vikit)

pods_install_headers(${h_files} ${hpp_files}
    DESTINATION vikit)
pods_install_libraries(vikit)
