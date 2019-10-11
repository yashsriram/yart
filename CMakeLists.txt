cmake_minimum_required(VERSION 3.5.0)

project("Ray Tracer")

# Flags
set(CMAKE_CXX_STANDARD 11)

# Sources and Headers
file(GLOB_RECURSE HEADERS "*hpp")

add_executable(raytracer raytracer.cpp)
