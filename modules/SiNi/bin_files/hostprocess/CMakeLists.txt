cmake_minimum_required (VERSION 3.8)
project(hostprocess VERSION 1.0)
set (CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILE 
*.cpp
)
file(GLOB HEADER_FILE 
*.h
)


add_executable(hostprocess WIN32
${SOURCE_FILE} 
${HEADER_FILE}
)


target_include_directories(hostprocess 
PRIVATE 
inc
)
