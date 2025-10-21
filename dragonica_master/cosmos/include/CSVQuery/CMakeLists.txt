project(CSVQuery)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

add_definitions(
    -DUNICODE
    -D_UNICODE
    -DWIN32
)

add_library(
    ${PROJECT_NAME} STATIC ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    CEL
    BM
    libcsv
    csvdb
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${PROJECT_FOLDER})
