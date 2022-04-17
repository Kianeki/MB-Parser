cmake_minimum_required(VERSION 3.17)
project(MB2)

set(CMAKE_CXX_STANDARD 14)

add_executable(MB2 main.cpp CFG.cpp json.hpp FSM.cpp ParseTable.cpp Parser.cpp Parser.h)