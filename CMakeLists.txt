cmake_minimum_required(VERSION 3.13)

set(SOURCES
    src/rendell_text.cpp
    src/TextRenderer.cpp
    src/TextBatch.cpp
    src/TextBuffer.cpp
    src/RasteredFontStorage.cpp
    src/RasteredFontStorageManager.cpp
    src/GlyphBuffer.cpp
    src/FontRaster.cpp
)

set(HEADERS
    src/rendell_text.h
    src/TextRenderer.h
    src/TextBatch.h
    src/TextBuffer.h
    src/RasteredFontStorage.h
    src/RasteredFontStorageManager.h
    src/GlyphBuffer.h
    src/FontRaster.h
)

add_library(rendell_text ${SOURCES} ${HEADERS})
target_include_directories(rendell_text PUBLIC include)

target_link_libraries(rendell_text PRIVATE ${RENDELL_LIB})
target_link_libraries(rendell_text PRIVATE ${FREETYPE_LIB})
target_link_libraries(rendell_text PRIVATE ${GLM_LIB})
