project(ContentsServer)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Item/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Contents/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Item/
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/
    ${CMAKE_CURRENT_SOURCE_DIR}/Contents/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../Weapons/
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/lib/
    ${CMAKE_CURRENT_SOURCE_DIR}/../Weapons/lib/
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

target_link_libraries(
    ${PROJECT_NAME}
    libcsv
    csvdb
    Collins
    Lohengrin
    Variant
    BM
    CEL
    RandomLib
    hotmeta
    CSMail
    tinyxml
    lzo
    CSVQuery
    lua
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /W3 /nologo /EHa
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/Zi;/RTC1;/DDEBUG;/D_DEBUG;/Zm395;/D_MDd_>"
    "$<$<CONFIG:Release>:/MD;/O2;/Zi;/Ot;/DNDEBUG;/Zm348;/D_MDo_>"
    "$<$<CONFIG:RelWithDebInfo>:/Zm348>"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER ${PROJECT_FOLDER}
    DEBUG_POSTFIX _Release
    RELWITHDEBINFO_POSTFIX _Release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/mmc/patch/contents/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/mmc/patch/contents/"
    COMPILE_FLAGS "/EHa /GF /Yustdafx.h"
)
