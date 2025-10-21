project(libcsv_usage)

include_directories(
    ${libcsv_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(
    ${PROJECT_NAME}
    libcsv
)