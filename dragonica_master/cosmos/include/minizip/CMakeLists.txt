project(minizip)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

add_definitions(
    -DWIN32
    -D_LIB
    -D_CRT_SECURE_NO_WARNINGS
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /WX
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG>"
)

COSMOS_STATIC_LIB_CONFIG_OUTPUT()