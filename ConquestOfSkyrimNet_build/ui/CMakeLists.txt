cmake_minimum_required(VERSION 3.21)
project(ConquestOfSkyrimNetUI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CommonLibSSE CONFIG REQUIRED)

add_commonlibsse_plugin(${PROJECT_NAME}
    NAME "ConquestOfSkyrimNetUI"
    AUTHOR "Conquest of SkyrimNet"
    SOURCES src/main.cpp
)

# PrismaUI_API.h goes in include/ (download it from the PrismaUI docs / example plugin).
target_include_directories(${PROJECT_NAME} PRIVATE include)

# CommonLib's generated plugin-info source relies on a precompiled header.
target_precompile_headers(${PROJECT_NAME} PRIVATE include/csn_pch.h)
# PrismaUI_API.h calls GetModuleHandle with a wide string.
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
