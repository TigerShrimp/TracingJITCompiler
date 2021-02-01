cmake_minimum_required(VERSION 3.19.0)
set (CMAKE_CXX_STANDARD 17)
project(Understandable_Tracing_JIT_Compiler)

include_directories(include)
 
file(GLOB SOURCES "src/*.cpp")
 
# Tell asmjit that it will be embedded.
Set(ASMJIT_EMBED TRUE)
Add_Definitions(-DASMJIT_STATIC)

Set(ASMJIT_DIR "libs/asmjit")
Include("${ASMJIT_DIR}/CMakeLists.txt")

# If you add a library / executable, include asmjit sources.
Add_Executable(tigershrimp ${SOURCES} ${ASMJIT_SRC})

# You still have to include asmjit dependencies.
Target_Link_Libraries(tigershrimp ${ASMJIT_DEPS})