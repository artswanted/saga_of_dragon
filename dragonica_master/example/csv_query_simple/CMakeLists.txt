project(csv_query_simple)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/include/
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cosmos/lib/
)

add_definitions(
    -DUNICODE
    -D_UNICODE
    -DWIN32
)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(
    ${PROJECT_NAME}
    libcsv
    tinyxml
    CEL
    lzo
    BM
    csvdb
    Lohengrin
    CSVQuery
)