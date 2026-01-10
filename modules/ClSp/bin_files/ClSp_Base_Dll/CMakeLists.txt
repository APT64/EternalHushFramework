cmake_minimum_required (VERSION 3.8)

project(ClSp_Base_Dll VERSION 1.0 LANGUAGES)
set (CMAKE_CXX_STANDARD 20)

file(GLOB SOURCE_FILE 
*.cpp
)
file(GLOB SHARED_SOURCE_FILE 
../shared/*.cpp
)

file(GLOB HEADER_FILE 
inc/*.h
)

if(${MSVC_C_ARCHITECTURE_ID} STREQUAL "x64")
	file(GLOB SHARED_ASM_FILE ../shared/asm/syscalls64.asm)
elseif(${MSVC_C_ARCHITECTURE_ID} STREQUAL "X86")
	file(GLOB SHARED_ASM_FILE ../shared/asm/syscalls32.asm)
endif()
add_library(ClSp_Base_Dll SHARED
${SOURCE_FILE}
${SHARED_SOURCE_FILE}
${HEADER_FILE}
${SHARED_ASM_FILE}
)

target_include_directories(ClSp_Base_Dll
PRIVATE 
inc
)