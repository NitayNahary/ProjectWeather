cmake_minimum_required(VERSION 3.21)
project(ProjectWeather)

set(CMAKE_CXX_STANDARD 14)

add_executable(ProjectWeather main.cpp Weather.cpp Weather.h httplib.h)
