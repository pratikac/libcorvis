file(GLOB hpp_files *.hpp)
file(GLOB cpp_files *.cpp)

add_executable(test_so2 test_so2.cpp)
add_executable(test_se2 test_se2.cpp)
add_executable(test_so3 test_so3.cpp)
add_executable(test_se3 test_se3.cpp)
add_executable(test_sim3 test_sim3.cpp)
add_executable(test_rxso3 test_rxso3.cpp)
