project(Dragonica)

file(GLOB SOURCES
    *.cpp
    NewWare/*.cpp
    NewWare/Game/*.cpp
    NewWare/Renderer/*.cpp
    NewWare/Renderer/Kernel/*.cpp
    NewWare/Renderer/OCuller/*.cpp
    NewWare/Renderer/OCuller/HW/*.cpp
    NewWare/Renderer/OCuller/SW/*.cpp
    NewWare/Renderer/Task/*.cpp
    NewWare/Scene/*.cpp
    NewWare/Tools/*.cpp
    NiApplication/*.cpp
    Mods/PgMods.cpp
    Utility/*.cpp
    Ranking/*.cpp
    lua_modules/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/DirectXSDK/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/PlatformSDK/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Gamebryo_2.2.2/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Include/BM/
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/include/FreeType
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/include/zlib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/PhysX/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Archangel/Weapons
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/FxStudio/Win32/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/NewWare/Game/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NewWare/Renderer/Kernel/
    ${CMAKE_CURRENT_SOURCE_DIR}/NewWare/Renderer/OCuller/
    ${CMAKE_CURRENT_SOURCE_DIR}/NewWare/Renderer/OCuller/HW/
    ${CMAKE_CURRENT_SOURCE_DIR}/NewWare/Renderer/OCuller/SW/
    ${CMAKE_CURRENT_SOURCE_DIR}/NewWare/Renderer/Task/
    ${CMAKE_CURRENT_SOURCE_DIR}/NewWare/Scene/
    ${CMAKE_CURRENT_SOURCE_DIR}/NewWare/Tools/
    ${CMAKE_CURRENT_SOURCE_DIR}/NiApplication/
    ${CMAKE_CURRENT_SOURCE_DIR}/Utility/
    ${CMAKE_CURRENT_SOURCE_DIR}/Ranking/
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/DirectXSDK/lib/x86
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/PlatformSDK/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/PhysX/Lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/FxStudio/Win32/Lib
)

set_source_files_properties(stdafx.cpp
    PROPERTIES
    COMPILE_FLAGS "/Ycstdafx.h"
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/mobilesuit.rc
)

target_link_libraries(${PROJECT_NAME}
    BM
    CEL
    DataPack
    SimpEnc
    Lohengrin
    Variant
    Onibal
    Collins
    RandomLib
    tinyxml
    XUI
    discord
    CSIME
    hotmeta
    lzo
    LuaDebuggerLIB
    lua_tinker
    lua
    Shiny
    zlib
    freetype
    loki
    CxImage
    drem_sys
    BugTrap
)

add_definitions(
    -DWIN32
    -DUNICODE
    -D_UNICODE
    -D_WINDOWS
    -DNI_RELEASE_MODE_LOGGING=1
)

# Do not use /ZI use /Zi flag, casue: error C2065: '__LINE__Var' : undeclared identifier
target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /W3 /Yustdafx.h /Zi /EHa /GF /nologo /DSTRICT /FINiBuildConfiguration.h
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/D_DEBUG;/DDEBUG;/DNIDEBUG;/D_MDd_;/Zm485>"
    "$<$<CONFIG:RelWithDebInfo>:/DNIRELEASE;/Zm433;/DUSE_GM>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/DEXTERNAL_RELEASE;/DUSE_INB;/D_MDo_;/DNISHIPPING;/Zm426>"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    DEBUG_POSTFIX _InternalD
    RELWITHDEBINFO_POSTFIX _Internal
    FOLDER "Freedom"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_exe/sfreedom_dev/release/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_exe/sfreedom_dev/release/"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_exe/sfreedom_dev/release/"
    LINK_FLAGS "/LARGEADDRESSAWARE /TSAWARE:NO /OPT:NOREF /OPT:NOICF /SUBSYSTEM:WINDOWS /nodefaultlib:libcmt /INCREMENTAL:NO"
    LINK_FLAGS_DEBUG "/libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Gamebryo_2.2.2/Lib/DebugLib /libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/FxStudio/Win32/Lib/DebugLib"
    LINK_FLAGS_RELWITHDEBINFO "/libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Gamebryo_2.2.2/Lib/ReleaseLib /libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/FxStudio/Win32/Lib/ReleaseLib"
    LINK_FLAGS_RELEASE "/libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Gamebryo_2.2.2/Lib/ShippingLib /libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/FxStudio/Win32/Lib/ShippingLib"
)
