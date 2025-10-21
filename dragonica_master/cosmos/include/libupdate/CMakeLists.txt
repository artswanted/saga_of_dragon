project(libupdate)

include_directories(
	.
)

set(SOURCES
    libupdate.cpp
)

add_definitions(
    -D_WIN32_WINNT=0x0501
    -DWIN32_LEAN_AND_MEAN
    -DBOOST_ASIO_ENABLE_CANCELIO
    # -DBOOST_ALL_STATIC_LINK
    -D_SCL_SECURE_NO_DEPRECATE
    -D_CRT_SECURE_NO_DEPRECATE
    -DTORRENT_BUILDING_LIBRARY
    -D_FILE_OFFSET_BITS=64
    -DBOOST_EXCEPTION_DISABLE
    -DBOOST_NO_EXCEPTIONS
    -DWITH_SHIPPED_GEOIP_H
    -DTORRENT_USE_TOMMATH
    -DTORRENT_VERBOSE_LOGGING
    -DTORRENT_LOGGING
    -DTORRENT_USE_LOCALE
)

# Use pre build for c++98 cause we have old libtorrent and boost
if (CMAKE_CXX_STANDARD MATCHES "98")

    add_library(${PROJECT_NAME} INTERFACE)

    target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libupdate.lib
    )

else()

    add_library(${PROJECT_NAME} SHARED ${SOURCES})

    target_link_libraries(${PROJECT_NAME}
        libtorrent
    )

endif()

