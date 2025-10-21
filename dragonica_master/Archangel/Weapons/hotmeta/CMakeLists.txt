project(hotmeta)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/D_MDd_>"
    # "$<$<CONFIG:RelWithDebInfo>:/GL>"
    "$<$<CONFIG:Release>:/MD;/Ob2;/Ot;/GL;/DNDEBUG;/D_MDo_>"
)

target_link_libraries(${PROJECT_NAME}
    Lohengrin
    CEL
    Variant
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${PROJECT_FOLDER})
