project(MMC)

file(GLOB SOURCES
    *.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../cosmos/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Weapons/
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../cosmos/lib/
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
    Variant
    BM
    CEL
    DataPack
    RandomLib
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    /Yustdafx.h
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/Zm348>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/Zm310>"
    "$<$<CONFIG:RelWithDebInfo>:/Zm310>"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER ${PROJECT_FOLDER}
    DEBUG_POSTFIX _Release
    RELWITHDEBINFO_POSTFIX _Release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../../../../dragonica_Exe/archangel_run/mmc/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../../dragonica_Exe/archangel_run/mmc/"
    LINK_FLAGS "/LARGEADDRESSAWARE /OPT:NOREF /OPT:NOICF"
)

