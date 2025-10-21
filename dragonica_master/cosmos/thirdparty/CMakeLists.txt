project(thirdparty)

# CURL Config
set(BUILD_CURL_EXE OFF)
set(BUILD_SHARED_LIBS OFF)
set(ENABLE_THREADED_RESOLVER OFF)
set(HTTP_ONLY ON)
set(CMAKE_USE_WINSSL ON)
add_subdirectory(curl)
add_subdirectory(discord_game_sdk)

set_target_properties(libcurl PROPERTIES FOLDER ${PROJECT_FOLDER})
set_target_properties(uninstall PROPERTIES FOLDER ${PROJECT_FOLDER})
