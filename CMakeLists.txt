cmake_minimum_required(VERSION 3.14)

project(RendellText)

set(CMAKE_CXX_STANDARD 23)

set(RENDELL_TEXT_RENDELL_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../rendell)
set(RENDELL_TEXT_LOGX_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../logx)

set(SOURCES
    src/TextLayout.cpp
    src/TextRenderer.cpp
    src/TextBatch.cpp
    src/TextBuffer.cpp
    src/GlyphBuffer.cpp
    src/RasteredFontStorage.cpp
    src/RasteredFontStorageManager.cpp
    src/FontRaster.cpp
    src/logging.cpp
)

set(HEADERS
    include/rendell_text/rendell_text.h
    include/rendell_text/defines.h
    include/rendell_text/TextLayout.h
    include/rendell_text/TextRenderer.h
    include/rendell_text/private/TextBatch.h
    include/rendell_text/private/TextBuffer.h
    include/rendell_text/private/GlyphBuffer.h
    include/rendell_text/private/IFontRaster.h
    include/rendell_text/private/FontRasterizationResult.h
    include/rendell_text/private/RasteredFontStorage.h
    internal/logging.h
    src/RasteredFontStorageManager.h
    src/FontRaster.h
    src/freetype.h
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

include(FetchContent)

# glm
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.0
)
FetchContent_MakeAvailable(glm)
target_link_libraries(rendell_text PRIVATE glm)

# LogX
FetchContent_Declare(
    logx
    SOURCE_DIR ${RENDELL_TEXT_LOGX_REPO_DIR}
    GIT_TAG ec691f85d22330f4ebd268ffcf6cc0ae7a7dd4f9
)
FetchContent_MakeAvailable(logx)
target_link_libraries(rendell_text PRIVATE logx)

# Rendell
FetchContent_Declare(
    rendell
    SOURCE_DIR ${RENDELL_TEXT_RENDELL_REPO_DIR}
    GIT_TAG 4d08c2416c71cad1e492fb5955ab4b09bb45d71b
)
FetchContent_MakeAvailable(rendell)
target_link_libraries(rendell_text PRIVATE rendell)

# FreeType
add_subdirectory(freetype)
target_link_libraries(rendell_text PRIVATE freetype)
