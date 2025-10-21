project(freetype)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/autofit/autofit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/bdf/bdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/cff/cff.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftbase.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftbitmap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/cache/ftcache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/ftdebug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftgasp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftglyph.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/gzip/ftgzip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftinit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/lzw/ftlzw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftstroke.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftsystem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/smooth/smooth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftbbox.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftmm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftpfr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftsynth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/fttype1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/base/ftwinfnt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/pcf/pcf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/pfr/pfr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/psaux/psaux.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/pshinter/pshinter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/psnames/psmodule.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/raster/raster.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/sfnt/sfnt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/truetype/truetype.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/type1/type1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/cid/type1cid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/type42/type42.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/src/winfonts/winfnt.c
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/include
    ${CMAKE_CURRENT_SOURCE_DIR}/freetype2.3.7/include/freetype
)

add_definitions(
    -DFT2_BUILD_LIBRARY
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(
    ${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:/MDd;/Od;/Ob0;/RTC1;/DDEBUG;/D_DEBUG;/D_MDd_>"
    "$<$<CONFIG:Release>:/MD;/O2;/Ot;/DNDEBUG;/D_MDo_>"
)

COSMOS_STATIC_LIB_CONFIG_OUTPUT()