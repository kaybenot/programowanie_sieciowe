cmake_minimum_required(VERSION 3.10.2)

set(PROJECT_NAME "program")
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/JSON.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/JSONValue.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} curl)
