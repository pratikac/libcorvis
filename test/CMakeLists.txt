add_executable(test_grabber
    test_grabber.cpp)

add_executable(test_tracker
    test_tracker.cpp)
target_link_libraries(test_tracker
    fast
    svo
    vikit)

add_executable(test_decode
    test_decode.cpp)

add_executable(test_image_receive
    test_image_receive.cpp)

add_executable(test_image_publish
    test_image_publish.cpp)

pods_install_executables(
    test_grabber
    test_tracker
    test_decode
    test_image_publish
    test_image_receive)
