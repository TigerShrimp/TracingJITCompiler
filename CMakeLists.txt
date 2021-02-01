cmake_minimum_required(VERSION 3.19.0)
set (CMAKE_CXX_STANDARD 17)
project(TigerShrimp)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

# Tell asmjit that it will be embedded.
Set(ASMJIT_EMBED TRUE)
Add_Definitions(-DASMJIT_STATIC)

Set(ASMJIT_DIR "libs/asmjit")
Include("${ASMJIT_DIR}/CMakeLists.txt")

include_directories("libs/asmjit/src")

# If you add a library / executable, include asmjit sources.
Add_Executable(TigerShrimp main.cpp ${SOURCES} ${ASMJIT_SRC})

# You still have to include asmjit dependencies.
Target_Link_Libraries(TigerShrimp ${ASMJIT_DEPS})