cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(LabDB VERSION 0.1.0 LANGUAGES CXX)

include_directories("include")

add_subdirectory("src")
add_subdirectory("test")