project(BugTrap)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

add_definitions(
    -DWIN32
    -D_WINDOWS
    -D_USRDLL
    -DBUGTRAP_EXPORTS
    -D_NEW_
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_source_files_properties(src/StdAfx.cpp
    PROPERTIES
    COMPILE_FLAGS "/YcStdAfx.h"
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /W4
    /YuStdAfx.h
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG>"
)

target_link_libraries(${PROJECT_NAME}
    zlib
    minizip
)

COSMOS_STATIC_LIB_CONFIG_OUTPUT()