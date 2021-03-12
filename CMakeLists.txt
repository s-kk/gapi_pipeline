cmake_minimum_required(VERSION 3.17)
project(G-API_Perf)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV 4 REQUIRED)
find_package(spdlog REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

add_executable(G-API_Perf main.cpp ImageProcess.cpp)
target_link_libraries(G-API_Perf ${OpenCV_LIBS})
target_link_libraries(G-API_Perf spdlog::spdlog)