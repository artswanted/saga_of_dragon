project(Patcher)

file(GLOB SOURCES
    *.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Include
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/lib
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/Patcher.rc
)

target_link_libraries(${PROJECT_NAME}
    BM
    CEL
    DataPack
    SimpEnc
    tinyxml
)

add_definitions(
    -DWIN32
    -DUNICODE
    -D_UNICODE
    -D_WINDOWS
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /W3 /Zi /EHa /GF /nologo /DSTRICT
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/D_DEBUG;/DDEBUG;/DNIDEBUG;/D_MDd_;/Zm485>"
    "$<$<CONFIG:RelWithDebInfo>:/DNIRELEASE;/D_MDo_;/Zm433>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/DEXTERNAL_RELEASE;/D_MDo_;/DDEFAULT;/Zm426>"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER ${PROJECT_FOLDER}
    DEBUG_POSTFIX _InternalD
    RELWITHDEBINFO_POSTFIX _Internal
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dragonica_Exe/ToolBin/Patcher/"
    LINK_FLAGS "/LARGEADDRESSAWARE /TSAWARE:NO /OPT:NOREF /OPT:NOICF /SUBSYSTEM:WINDOWS /nodefaultlib:libcmt /INCREMENTAL:NO"
    LINK_FLAGS_DEBUG "/libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Gamebryo_2.2.2/Lib/DebugLib /libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/FxStudio/Win32/Lib/DebugLib"
    LINK_FLAGS_RELWITHDEBINFO "/libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Gamebryo_2.2.2/Lib/ReleaseLib /libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/FxStudio/Win32/Lib/ReleaseLib"
    LINK_FLAGS_RELEASE "/libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/Gamebryo_2.2.2/Lib/ShippingLib /libpath:${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/FxStudio/Win32/Lib/ShippingLib"
)
