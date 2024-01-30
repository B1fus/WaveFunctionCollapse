cmake_minimum_required(VERSION 3.22)
project(mapGenerator)

set (CMAKE_CXX_STANDARD 23)
set (CXX_STANDARD 23)

find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

set (SRC
    "src/main.cpp"
    "src/tilesMap.cpp"
)

set (INCLUDE
    "include/tilesMap.h"
)


add_executable(generator ${SRC})

target_link_libraries( generator ${OpenCV_LIBS} )
