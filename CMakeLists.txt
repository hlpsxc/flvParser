cmake_minimum_required(VERSION 3.1)
project(flvparser)
set(CMAKE_CXX_STANDARD 11)
add_executable(flvparser src/BitReadStream.cpp src/BitWriteStream.cpp src/main.cpp src/FlvParser.cpp)
