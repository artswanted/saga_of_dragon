project(csvdb)

file(GLOB SOURCES
    src/*.cpp
    src/def/*.cpp
)

include_directories(
    ${libcsv_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Archangel/Weapons
)

add_definitions(
    -DUNICODE
    -D_UNICODE_
    -D_UNICODE
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set(${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/ CACHE INTERNAL "CSV DB include dir")

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${PROJECT_FOLDER})
