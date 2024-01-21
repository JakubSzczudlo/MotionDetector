cmake_minimum_required(VERSION 2.8.9)
project(unknown_detector)

include(CTest)

include_directories("inc"
# "src/os"
"src/camera"
"src/client"
# "src/detector"
# "src/utilities"
)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using default GCC C compiler")
    add_compile_options(
        -Wall
        -Wextra
    )
elseif(CMAKE_C_COMPILER STREQUAL "Clang")
    message(STATUS "Using default Clang C compiler")
    add_compile_options(
        -Weverything
    )
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(OpenCV_DIR "../../build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(unknown_detector ${SOURCES})
target_link_libraries(unknown_detector PRIVATE pthread)
target_link_libraries(unknown_detector PRIVATE ${OpenCV_LIBS})



# add_library(utilities src/utilities/utilities.cpp)
# add_executable(testModel test/testModel.c)
# target_link_libraries(testModel PRIVATE utilities)

# add_test(
#     NAME MyTest
#     COMMAND testModel
# )
