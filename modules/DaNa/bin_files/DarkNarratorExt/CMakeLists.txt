cmake_minimum_required (VERSION 3.8)

project(DarkNarratorExt VERSION 1.0 LANGUAGES)
set (CMAKE_CXX_STANDARD 23)

file(GLOB SOURCE_FILE 
*.*pp
)
file(GLOB SHARED_SOURCE_FILE 
../../../ClSp/bin_files/shared/*.cpp

)

file(GLOB HEADER_FILE 
inc/*.h
)

if(${MSVC_C_ARCHITECTURE_ID} STREQUAL "x64")
	file(GLOB SHARED_ASM_FILE ../../../ClSp/bin_files/shared/asm/syscalls64.asm)
elseif(${MSVC_C_ARCHITECTURE_ID} STREQUAL "X86")
	file(GLOB SHARED_ASM_FILE ../../../ClSp/bin_files/shared/asm/syscalls32.asm)
endif()

add_library(DarkNarratorExt SHARED
${SOURCE_FILE}
${SHARED_SOURCE_FILE}
${HEADER_FILE}
${SHARED_ASM_FILE}
)

target_include_directories(DarkNarratorExt
PRIVATE 
inc
../../../ClSp/bin_files/shared/inc
)