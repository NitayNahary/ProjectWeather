cmake_minimum_required(VERSION 3.21)
project(WeatherProject)

set(CMAKE_CXX_STANDARD 14)

# Add the main executable
add_executable(WeatherProject main.cpp Weather.h Weather.cpp)

# Include the directory containing httplib.h
target_include_directories(WeatherProject PRIVATE ${CMAKE_SOURCE_DIR})

# Link against the Winsock library
target_link_libraries(WeatherProject PRIVATE ws2_32)