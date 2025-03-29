cmake_minimum_required(VERSION 3.13)

set(SOURCES
    src/TextLayout.cpp
    src/TextRenderer.cpp
    src/TextBatch.cpp
    src/TextBuffer.cpp
    src/GlyphBuffer.cpp
    src/RasteredFontStorage.cpp
    src/RasteredFontStorageManager.cpp
    src/FontRaster.cpp
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

include_directories(include)
add_library(rendell_text ${SOURCES} ${HEADERS})
target_include_directories(rendell_text PUBLIC include)

target_link_libraries(rendell_text PRIVATE ${RENDELL_LIB})
target_link_libraries(rendell_text PRIVATE ${FREETYPE_LIB})
target_link_libraries(rendell_text PRIVATE ${GLM_LIB})
