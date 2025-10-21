project(ActorEventSystem)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

include_directories(
    ../
    ../../../cosmos/include/
)

add_definitions(
    -DWIN32
    -DUNICODE
    -D_UNICODE
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    BM
    Variant
    Lohengrin
    Onibal
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${PROJECT_FOLDER})
