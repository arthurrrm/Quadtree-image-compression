cmake_minimum_required(VERSION 3.10)
project(QuadtreeImageCompression)

find_package(OpenCV REQUIRED) 
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
add_executable(quadtree_compression ${SOURCES})
target_link_libraries(quadtree_compression PRIVATE ${OpenCV_LIBS})

# Optional: Add tests
enable_testing()
add_executable(tests tests/test_main.cpp src/Quadtree.cpp)
target_link_libraries(tests PRIVATE ${OpenCV_LIBS})
add_test(NAME QuadtreeTests COMMAND tests) 