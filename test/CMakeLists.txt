add_executable(test_grabber
    test_grabber.cpp)

add_executable(test_tracker
    test_tracker.cpp)
target_link_libraries(test_tracker
    fast
    svo
    vikit)

pods_install_executables(
    test_grabber
    test_tracker)
