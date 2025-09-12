cmake_minimum_required(VERSION 3.22)

project(randutils CXX)

add_library(randutils INTERFACE include/randutils.hpp)

target_include_directories(randutils INTERFACE include)
