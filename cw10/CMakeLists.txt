cmake_minimum_required(VERSION 3.10.2)

set(PROJECT_NAME "program")
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} curl)
