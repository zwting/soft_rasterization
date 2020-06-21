cmake_minimum_required(VERSION 3.5)

project(soft_rasterization)
set(CMAKE_CXX_STANDARD 11)

set(third_part_dir ${PROJECT_SOURCE_DIR}/third_part)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${third_part_dir})

# opencv
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(SOURCE_LIST
        main.cpp
        src/core/frame_buffer.cpp
        src/core/renderer.cpp
        third_part/tgaimage.cpp
        src/misc/geometry_utils.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_LIST})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

