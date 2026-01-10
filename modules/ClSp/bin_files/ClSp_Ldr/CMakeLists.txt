cmake_minimum_required (VERSION 3.8)

project(ClSp_Ldr VERSION 1.0 LANGUAGES)
set (CMAKE_CXX_STANDARD 20)

add_subdirectory(http_exe)
add_subdirectory(tcp_exe)

file(GLOB SOURCE_FILE 
dispatcher.cpp
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
###################################[HTTP EXE]#####################################
file(GLOB HTTP_SRC_FILE 
http_exe/*.cpp
startmain.cpp
)
file(GLOB HTTP_HEADER_FILE 
http_exe/inc/*.h
)
add_executable(ClSp_Http_Exe WIN32
${SOURCE_FILE}
${SHARED_SOURCE_FILE}
${HEADER_FILE}
${SHARED_ASM_FILE}
${HTTP_SRC_FILE}
${HTTP_HEADER_FILE}
)

target_include_directories(ClSp_Http_Exe 
PRIVATE 
inc
)
#target_link_libraries(ClSp_Http_Exe http_exe)
###################################[TCP EXE]######################################
file(GLOB TCP_SRC_FILE 
tcp_exe/*.cpp
startmain.cpp
)
file(GLOB TCP_HEADER_FILE 
tcp_exe/inc/*.h
)
add_executable(ClSp_Tcp_Exe WIN32
${SOURCE_FILE}
${SHARED_SOURCE_FILE}
${HEADER_FILE}
${SHARED_ASM_FILE}
${TCP_SRC_FILE}
${TCP_HEADER_FILE}
)
target_include_directories(ClSp_Tcp_Exe 
PRIVATE
inc
)
#target_link_libraries(ClSp_Tcp_Exe tcp_exe)
###################################[HTTP DLL]######################################
file(GLOB HTTP_DLL_SRC_FILE 
http_exe/*.cpp
dllexport.cpp
)
file(GLOB HTTP_DLL_HEADER_FILE 
http_exe/inc/*.h
)
add_library(ClSp_Http_Dll SHARED
${SOURCE_FILE}
${SHARED_SOURCE_FILE}
${HEADER_FILE}
${SHARED_ASM_FILE}
${HTTP_DLL_SRC_FILE}
${HTTP_DLL_HEADER_FILE}
)

target_include_directories(ClSp_Http_Dll 
PRIVATE 
inc
)
#target_link_libraries(ClSp_Http_Exe http_exe)
###################################[TCP DLL]######################################
file(GLOB TCP_DLL_SRC_FILE 
tcp_exe/*.cpp
dllexport.cpp
)
file(GLOB TCP_DLL_HEADER_FILE 
tcp_exe/inc/*.h
)
add_library(ClSp_Tcp_Dll SHARED
${SOURCE_FILE}
${SHARED_SOURCE_FILE}
${HEADER_FILE}
${SHARED_ASM_FILE}
${TCP_DLL_SRC_FILE}
${TCP_DLL_HEADER_FILE}
)
target_include_directories(ClSp_Tcp_Dll 
PRIVATE
inc
)
#target_link_libraries(ClSp_Tcp_Exe tcp_exe)