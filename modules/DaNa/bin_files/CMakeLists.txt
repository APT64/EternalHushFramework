cmake_minimum_required (VERSION 3.8)

enable_language(C ASM_NASM)
set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
set(CMAKE_SHARED_LINKER_FLAGS /MANIFEST:NO)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /GS-")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /GS-")
set(CMAKE_ASM_FLAGS "${CFLAGS} -x assembler-with-cpp")

add_subdirectory ("DarkNarratorExt")