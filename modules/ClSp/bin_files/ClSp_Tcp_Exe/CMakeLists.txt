cmake_minimum_required (VERSION 3.8)

project(ClSp_Tcp_Exe VERSION 1.0)
set (CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILE 
*.cpp
)
file(GLOB HEADER_FILE 
inc/*.h*
)

add_executable(ClSp_Tcp_Exe WIN32
${SOURCE_FILE} 
${HEADER_FILE}
)

target_include_directories(ClSp_Tcp_Exe 
PRIVATE 
inc
)