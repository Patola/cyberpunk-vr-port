cmake_minimum_required(VERSION 3.20)
project(CyberpunkVR_Hands)

set(CMAKE_CXX_STANDARD 20)

set(CPVR_CASE_SENSITIVE_INCLUDE_SHIMS OFF)
if(MINGW OR (MSVC AND CMAKE_HOST_UNIX))
    set(CPVR_CASE_SENSITIVE_INCLUDE_SHIMS ON)
endif()

# Add RED4ext.SDK
add_subdirectory(${CMAKE_SOURCE_DIR}/../../externals/RED4ext.SDK _deps/red4ext)

# MinHook (compiled directly from source — no CMakeLists of its own)
set(MINHOOK_DIR ${CMAKE_SOURCE_DIR}/../../externals/minhook)
add_library(minhook STATIC
    ${MINHOOK_DIR}/src/buffer.c
    ${MINHOOK_DIR}/src/hook.c
    ${MINHOOK_DIR}/src/trampoline.c
    ${MINHOOK_DIR}/src/hde/hde32.c
    ${MINHOOK_DIR}/src/hde/hde64.c
)
target_include_directories(minhook PUBLIC ${MINHOOK_DIR}/include)

add_library(CyberpunkVR_Hands SHARED main.cpp)
target_link_libraries(CyberpunkVR_Hands PRIVATE RED4ext.SDK minhook)
if(CPVR_CASE_SENSITIVE_INCLUDE_SHIMS)
    set(CPVR_INCLUDE_SHIM_DIR "${CMAKE_SOURCE_DIR}/../../cmake/mingw-include")
    target_include_directories(CyberpunkVR_Hands PRIVATE ${CPVR_INCLUDE_SHIM_DIR})
    target_include_directories(RED4ext.SDK PUBLIC ${CPVR_INCLUDE_SHIM_DIR})
endif()

# Rename to .dll but standard prefix is none
set_target_properties(CyberpunkVR_Hands PROPERTIES PREFIX "")
