add_executable(usb_camera_driver
    usb_camera_driver.cpp)
target_link_libraries(usb_camera_driver
    lcmtypes_corvis)

add_executable(imu_driver
    imu_driver.cpp)
target_link_libraries(imu_driver
    lcmtypes_corvis)

pods_install_executables(usb_camera_driver imu_driver)
