file(GLOB h_files *.h)
file(GLOB cpp_files *.cpp)

add_library(svo
    STATIC ${cpp_files})
target_link_libraries(svo
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    fast
    vikit)

pods_install_headers(${h_files}
    DESTINATION svo)
pods_install_libraries(svo)
