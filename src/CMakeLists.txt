file(GLOB h_files *.h)
file(GLOB cpp_files *.cpp)

pods_install_headers(${h_files}
    DESTINATION corvis)


add_subdirectory(viewer)