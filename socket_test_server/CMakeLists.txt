cmake_minimum_required(VERSION 2.8.9)
project(socket)
set(PROJECT_NAME detection_server)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED True)

set(OpenCV_DIR "/usr/local/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})



if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using default GCC C compiler")
    add_compile_options(
        -Wall
        -Wextra
    )
endif()