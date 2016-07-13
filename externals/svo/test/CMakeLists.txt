add_executable(test_feature_align
    test_feature_align.cpp)
target_link_libraries(test_feature_align
    svo)

add_executable(test_pipeline
    test_pipeline.cpp)
target_link_libraries(test_pipeline
    svo)

add_executable(test_matcher
    test_matcher.cpp)
target_link_libraries(test_matcher
    svo)

add_executable(test_feature_detection
    test_feature_detection.cpp)
target_link_libraries(test_feature_detection
    svo)

add_executable(test_depth_filter
    test_depth_filter.cpp)
target_link_libraries(test_depth_filter
    svo)

add_executable(test_sparse_img_align
    test_sparse_img_align.cpp)
target_link_libraries(test_sparse_img_align
    svo)

add_executable(test_pose_optimizer
    test_pose_optimizer.cpp)
target_link_libraries(test_pose_optimizer
    svo)

pods_install_executables(test_feature_align
    test_pipeline
    test_matcher
    test_feature_detection
    test_depth_filter
    test_sparse_img_align
    test_pose_optimizer)
