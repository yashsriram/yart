cmake_minimum_required(VERSION 3.5.0)

# Flags
set(CMAKE_CXX_STANDARD 11)

# Sources and Headers
file(GLOB_RECURSE HEADERS "*hpp")

add_executable(ray_caster.cpp ray_caster)