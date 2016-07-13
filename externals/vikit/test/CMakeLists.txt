add_executable(test_camera
    test_camera.cpp)
target_link_libraries(test_camera
    vikit)

add_executable(test_triangulation
    test_triangulation.cpp)
target_link_libraries(test_triangulation
    vikit)

add_executable(test_patch_score
    test_patch_score.cpp)
target_link_libraries(test_patch_score
    vikit)

pods_install_executables(test_camera
    test_triangulation
    test_patch_score)
