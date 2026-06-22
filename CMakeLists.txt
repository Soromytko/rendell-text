cmake_minimum_required(VERSION 3.14)

project(RendellText)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES
    src/initialization.cpp
    src/font.cpp
    src/logging.cpp
    src/factory.cpp

    src/GlyphRaster.cpp

    src/HarfBuzzShaper.cpp
    src/FontFallbackResolver.cpp
    src/FontLibrary.cpp
    
    src/SkylineGlyphAtlas.cpp
    src/GlyphMultiAtlas.cpp

    src/utils/BmpSaver.cpp
)

set(HEADERS
    include/rendell_text/types.h
    include/rendell_text/font.h
    include/rendell_text/initialization.h
    include/rendell_text/factory.h
    include/rendell_text/ShaperConfig.h
    include/rendell_text/Glyph.h
    include/rendell_text/TextRun.h
    include/rendell_text/IFontMetricsProvider.h
    include/rendell_text/IGlyphShaper.h
    include/rendell_text/ITextBuffer.h
    include/rendell_text/IGlyphAtlasTexture.h
    include/rendell_text/IGlyphAtlas.h
    include/rendell_text/IGlyphMultiAtlas.h
    include/rendell_text/IFontFallbackResolver.h
    include/rendell_text/IGlyphRaster.h
    include/rendell_text/ShapedGlyph.h
    include/rendell_text/RasterizedGlyph.h

    internal/logging.h
    internal/SkylineGlyphAtlas.h
    internal/GlyphMultiAtlas.h
    internal/HarfBuzzShaper.h
    internal/FontFallbackResolver.h
    internal/GlyphRaster.h

    internal/GlyphKey.h

    internal/FontLibrary.h
    internal/IFontData.h
    internal/HB_Resource.h
    internal/MSDF_Resource.h
    internal/SharedFontData.h

    internal/freetype.h

    internal/utils/BmpSaver.h
)

set(SHADERS
    res/Shaders/TextRenderer.vs
    res/Shaders/TextRenderer.fs
)

set(GENERATED_SHADER_OUTPUT_DIR generated_shader_headers)

function(generate_shader_header_file input_file output_dir)
    file(READ "${input_file}" file_content)
    string(REGEX REPLACE "[/\\.]" "_" variable_name "${input_file}")
    set(new_content "#pragma once\nstatic const char* ${variable_name} = R\"(\n${file_content}\n)\";")
    set(file_name "${variable_name}.h")
    file(WRITE "${output_dir}/${file_name}" "${new_content}")
endfunction()

function(generate_shader_header_files)
    foreach(shader_file IN LISTS SHADERS)
        generate_shader_header_file(${shader_file} ${GENERATED_SHADER_OUTPUT_DIR})
    endforeach()
endfunction()

generate_shader_header_files()
include_directories(${GENERATED_SHADER_OUTPUT_DIR})

add_library(rendell_text STATIC ${SOURCES} ${HEADERS})
target_include_directories(rendell_text PUBLIC include)
target_include_directories(rendell_text PRIVATE internal)

# logr
if(NOT TARGET logr)
    add_subdirectory(logr)
endif()
target_link_libraries(rendell_text PRIVATE logr)

# FreeType
add_subdirectory(freetype)
add_library(Freetype::Freetype ALIAS freetype)
target_link_libraries(rendell_text PRIVATE freetype)

# tinyxml2
add_subdirectory(tinyxml2)
add_library(tinyxml2::tinyxml2 ALIAS tinyxml2)

# msdfgen
set(MSDFGEN_USE_VCPKG OFF CACHE BOOL "" FORCE)
set(MSDFGEN_CORE_ONLY OFF CACHE BOOL "" FORCE)
set(MSDFGEN_USE_SKIA OFF CACHE BOOL "" FORCE)
set(MSDFGEN_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
set(MSDFGEN_DISABLE_PNG ON CACHE BOOL "" FORCE)
add_subdirectory(msdfgen)
target_link_libraries(rendell_text PRIVATE msdfgen-core msdfgen-ext)

# HarfBuzz
add_subdirectory(harfbuzz)
target_link_libraries(rendell_text PRIVATE harfbuzz)
