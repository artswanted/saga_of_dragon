project(CxImage)

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
    -D_LIB
    -DJAS_WIN_MSVC_BUILD
    -D_CRT_SECURE_NO_DEPRECATE
    -D_CRT_NON_CONFORMING_SWPRINTFS
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /W2 /WX
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG>"
)

target_link_libraries(${PROJECT_NAME}
    jpeg
)

COSMOS_STATIC_LIB_CONFIG_OUTPUT()