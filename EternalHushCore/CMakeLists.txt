cmake_minimum_required (VERSION 3.8)

project(EternalHushCore VERSION 1.0)
set (CMAKE_CXX_STANDARD 17)

# Включите подпроекты.
add_subdirectory(src)

#python api
set(PYTHON_INCLUDE_DIRS inc/python)
set(PYTHON_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/libs/python39.lib)

#winsock
set(WINSOCK_LIBRARY ws2_32)

file(GLOB SOURCE_FILE 
src/*.c*
)

add_library(EternalHushCore SHARED
${SOURCE_FILE}
)

target_link_libraries(EternalHushCore 
PRIVATE 
${PYTHON_LIBRARY}
${WINSOCK_LIBRARY}
)

target_include_directories(EternalHushCore 
PRIVATE 
inc
${PYTHON_INCLUDE_DIRS}
)