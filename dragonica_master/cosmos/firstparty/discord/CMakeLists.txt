project(discord)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    discord_game_sdk
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER ${PROJECT_FOLDER}
    DEBUG_POSTFIX -d
)
