cmake_minimum_required(VERSION 2.8.9)
project(socket)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED True)

set(OpenCV_DIR "../../build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(unknown_detector main.cpp)
target_link_libraries(unknown_detector PRIVATE pthread)
target_link_libraries(unknown_detector PRIVATE ${OpenCV_LIBS})



if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using default GCC C compiler")
    add_compile_options(
        -Wall
        -Wextra
    )
endif()