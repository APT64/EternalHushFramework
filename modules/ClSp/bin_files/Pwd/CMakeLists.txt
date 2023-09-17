cmake_minimum_required (VERSION 3.8)

project(Pwd VERSION 1.0)
set (CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILE 
*.cpp
)
file(GLOB HEADER_FILE 
inc/*.h
)


add_library(Pwd SHARED
${SOURCE_FILE} 
${HEADER_FILE}
)

target_include_directories(Pwd 
PRIVATE 
inc
)
