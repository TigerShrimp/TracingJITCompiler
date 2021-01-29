cmake_minimum_required(VERSION 3.19.0)
set (CMAKE_CXX_STANDARD 17)
project(Understandable_Tracing_JIT_Compiler)
 
#Bring the headers, such as Student.h into the project
include_directories(include)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")
 
add_executable(tigershrimp ${SOURCES})