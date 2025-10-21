project(Lohengrin)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lua_modules/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../cosmos/include/
)

add_definitions(
    -DWIN32
    -DUNICODE
    -D_UNICODE
)

set_source_files_properties(stdafx.cpp
    PROPERTIES
    COMPILE_FLAGS "/Ycstdafx.h"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    FCS
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /Yustdafx.h
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/D_MDd_>"
    # "$<$<CONFIG:RelWithDebInfo>:/GL>"
    "$<$<CONFIG:Release>:/MD;/Ot;/Ob2;/GL;/DNDEBUG;/D_MDo_>"
)

WEAPONS_STATIC_LIB_CONFIG_OUTPUT()
