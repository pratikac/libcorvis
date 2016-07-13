add_executable(test_camera
    test_camera.cpp)

add_executable(test_triangulation
    test_triangulation.cpp)

add_executable(test_patch_score
    test_patch_score.cpp)

pods_install_executables(test_camera
    test_triangulation
    test_patch_score)
