project(SwitchServer)

file(GLOB SOURCES
    *.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/GameGuard/
    ${CMAKE_CURRENT_SOURCE_DIR}/HackShield_UNAUTO/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../Weapons/
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/GameGuard/ # For game guard lib
    ${CMAKE_CURRENT_SOURCE_DIR}/HackShield_UNAUTO/ # For game guard lib
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
    Variant
    FCS
    HellDart
    Onibal
    BM
    CEL
    RandomLib
    loki
    zlib
    tinyxml
    lua_tinker
    LuaDebuggerLIB
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    /Yustdafx.h
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/Zm349>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/Zm311>"
    "$<$<CONFIG:RelWithDebInfo>:/Zm311>"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER ${PROJECT_FOLDER}
    DEBUG_POSTFIX _Release
    RELWITHDEBINFO_POSTFIX _Release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/mmc/patch/switch/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/mmc/patch/switch/"
    LINK_FLAGS "/LARGEADDRESSAWARE /OPT:NOREF /OPT:NOICF /NODEFAULTLIB:libcmt"
)

