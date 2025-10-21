project(Shiny)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_definitions(
    -DWIN32
    -DUNICODE
    -D_UNICODE
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/D_MDd_>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/D_MDo_>"
)

COSMOS_STATIC_LIB_CONFIG_OUTPUT()