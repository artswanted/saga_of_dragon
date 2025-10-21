project(ImmigrationServer)

file(GLOB SOURCES
    *.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/NC/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../Weapons/
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/lib/
)

add_definitions(
    -DWIN32
    -D_CONSOLE
    -DUNICODE
    -D_UNICODE
)

set_source_files_properties(stdafx.cpp
    PROPERTIES
    COMPILE_FLAGS "/Ycstdafx.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    Collins
    Lohengrin
    FCS
    Variant
    HellDart
    CSMail
    BM
    CEL
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /W3 /nologo /EHa /GF /Zi /Yustdafx.h
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/Zm394>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/Zm347>"
    "$<$<CONFIG:RelWithDebInfo>:/Zm347>"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER ${PROJECT_FOLDER}
    DEBUG_POSTFIX _Release
    RELWITHDEBINFO_POSTFIX _Release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/mmc/patch/immigration/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/mmc/patch/immigration/"
)

