file(GLOB cpp_files
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(fp ${cpp_files})
    string(REPLACE ".cpp" "" _fp ${fp})
    add_executable(${_fp}
        ${fp})
    target_link_libraries(${_fp}
        fast svo vikit)
    pods_install_executables(${_fp})
endforeach()
