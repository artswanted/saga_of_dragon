project(BM)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Header/
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

add_definitions(
    -DWIN32
    -DUNICODE
    -D_UNICODE
    -DENABLE_MUTEX_PROFILE
)

set_source_files_properties(src/stdafx.cpp
    PROPERTIES
    COMPILE_FLAGS "/Ycstdafx.h"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /Yustdafx.h
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/D_MDd_>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/D_MDo_;/DENABLE_MUTEX_PROFILE>"
)

target_link_libraries(
    ${PROJECT_NAME}
    lzo
    ace
    loki
    zlib
    minizip
    tinyxml
)

COSMOS_STATIC_LIB_CONFIG_OUTPUT()