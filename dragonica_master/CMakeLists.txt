## BUILD
# cmake ./ -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=98 -B./build/ -G"Visual Studio 8 2005"  -A Win32
# cmake --build ./build

cmake_minimum_required(VERSION 3.11...3.11.4 FATAL_ERROR)

project(freedom)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmake/buildconfig.cmake)

option(BUILD_EXAMPLE "Build example project" OFF)
option(BUILD_TOOLS "Build Dev Tools" OFF)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDREM_DEV")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDREM_DEV")

if(MSVC)
    add_compile_options(/MP /W3 /nologo /EHa /GF /Zi)
    add_compile_options("$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/D_MDd_>")
    add_compile_options("$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/D_MDo_>")
    add_compile_options("$<$<CONFIG:RelWithDebInfo>:/MD;/Od;/Ob0;/DNDEBUG;/D_MD_>")
    set(CMAKE_EXE_LINKER_FLAGS "/LARGEADDRESSAWARE /OPT:NOREF /OPT:NOICF /DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
endif()

add_subdirectory(cosmos)
add_subdirectory(Archangel)
add_subdirectory(freedom/MobileSuit)

if (BUILD_TOOLS)
    add_subdirectory(Tools)
endif(BUILD_TOOLS)

if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif(BUILD_EXAMPLE)