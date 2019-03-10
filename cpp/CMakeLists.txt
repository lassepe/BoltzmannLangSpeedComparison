cmake_minimum_required(VERSION 3.5)
project(BoltzmannCppSpeedTest)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(speed_test src/speed_test.cpp)
target_link_libraries (speed_test Eigen3::Eigen)
