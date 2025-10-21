project(Dummy)

file(GLOB SOURCES
    Dummy.cpp
    Network.cpp
    KeyEvent.cpp
    # DummyClient.cpp
    # DummyClientMgr.cpp
    Global.cpp
    DummyClient2.cpp
    DummyMgr2.cpp
    stdafx.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    FCS
    Variant
    RandomLib
    Collins
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /W3 /nologo /EHa
    "$<$<CONFIG:Debug>:/Zm395>"
    "$<$<CONFIG:Release>:/Zm348>"
    "$<$<CONFIG:RelWithDebInfo>:/Zm348>"
)



set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER ${PROJECT_FOLDER}
    DEBUG_POSTFIX _Release
    RELWITHDEBINFO_POSTFIX _Release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/Dummy/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../dragonica_Exe/archangel_run/Dummy/"
    COMPILE_FLAGS "/EHa /GF /Yustdafx.h"
)
