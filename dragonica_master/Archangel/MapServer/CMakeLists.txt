project(MapServer)

file(GLOB SOURCES
    *.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AI/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AS/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PublicMap/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Effect/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SkillEffectAbilSystem/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/actions/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/actions/**/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SkillEffectAbilSystem/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/gamebryo_2.2.2/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/physx/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Weapons/
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/lib/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/physx/lib/
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
    Onibal
    BM
    CEL
    RandomLib
    drem_sys
    hotmeta
    ActorEventSystem
    lzo
    tinyxml
    lua
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    /Yustdafx.h
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_MDd_;/D_DEBUG;/DNIDEBUG;/D_CHECK_PUBLIC_MAP;/Zm422>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/DNISHIPPING;/D_MDo_;/Zm374>"
    "$<$<CONFIG:RelWithDebInfo>:/DNIRELEASE;/Zm374>"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER ${PROJECT_FOLDER}
    DEBUG_POSTFIX _Release
    RELWITHDEBINFO_POSTFIX _Release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/mmc/patch/map/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/mmc/patch/map/"
    LINK_FLAGS "/LARGEADDRESSAWARE /OPT:NOREF /OPT:NOICF"
    LINK_FLAGS_DEBUG "/LIBPATH:${CMAKE_CURRENT_SOURCE_DIR}/../../Cosmos/gamebryo_2.2.2/Lib/debuglib"
    LINK_FLAGS_RELEASE "/LIBPATH:${CMAKE_CURRENT_SOURCE_DIR}/../../Cosmos/gamebryo_2.2.2/Lib/shippinglib"
    LINK_FLAGS_RELWITHDEBINFO "/LIBPATH:${CMAKE_CURRENT_SOURCE_DIR}/../../Cosmos/gamebryo_2.2.2/Lib/ReleaseLib"
)

