project(tinyxml)

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

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    /WX
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG>"
)

COSMOS_STATIC_LIB_CONFIG_OUTPUT()
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    DEBUG_POSTFIX stl_mdd
    RELEASE_POSTFIX stl_mdo
    RELWITHDEBINFO_POSTFIX stl_md
)
