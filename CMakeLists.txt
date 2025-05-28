cmake_minimum_required(VERSION 3.10)
project(QuadtreeImageCompression)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
add_executable(quadtree_compression ${SOURCES})

# Optional: Add tests
enable_testing()
add_executable(tests tests/test_main.cpp src/Quadtree.cpp src/Image.cpp)
add_test(NAME QuadtreeTests COMMAND tests) 