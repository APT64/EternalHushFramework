cmake_minimum_required (VERSION 3.8)

set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
set(CMAKE_SHARED_LINKER_FLAGS /MANIFEST:NO)

add_subdirectory ("ClSp_Tcp_Exe")
add_subdirectory ("KeyGen")
add_subdirectory ("Whoami")
add_subdirectory ("Pwd")
add_subdirectory ("Exec")