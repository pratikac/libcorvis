add_executable(driver_usb_camera
    driver_usb_camera.cpp)
target_link_libraries(driver_usb_camera
    lcmtypes_corvis)

add_executable(driver_imu
    driver_imu.cpp)
target_link_libraries(driver_imu
    lcmtypes_corvis)

pods_install_executables(driver_usb_camera driver_imu)
