#pragma once
#include <rendell_text/types.h>

#include <msdfgen.h>

namespace rendell_text {
void saveToBmp(const float *pixelData, Size size, const char *filename);

void saveToBmp(const msdfgen::Bitmap<float, 3> &bitmap, const char *filename);
} // namespace rendell_text
