add_definitions(-g)

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

add_executable(test_lcm_camera_decode
    test_lcm_camera_decode.cpp)

add_executable(test_renderer_camera
    test_renderer_camera.cpp)

pods_install_executables(
    test_grabber
    test_tracker
    test_decode
    test_lcm_camera_decode
    test_renderer_camera)
