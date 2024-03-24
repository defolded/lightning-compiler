cmake_minimum_required(VERSION 3.22)

project(lightning)

set(CMAKE_CXX_STANDARD 23)

add_executable(lt src/main.cpp
        src/tokenize.h
        src/parse.h
        src/generate.h)