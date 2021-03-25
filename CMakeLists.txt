cmake_minimum_required(VERSION 3.19.0)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules")
project(TigerShrimp)

find_package(fmt)

include_directories(include include/JVM)

file(GLOB SOURCES "src/*.cpp" "src/JVM/*.cpp" "src/utils/*.cpp")

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
target_link_libraries(TigerShrimp fmt::fmt)