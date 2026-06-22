#include <utils/BmpSaver.h>

#include <fstream>
#include <vector>

namespace rendell_text {
void saveToBmp(const float *pixelData, Size size, const char *filename) {
    int width = size.width;
    int height = size.height;

    int rowSize = (width * 3 + 3) & ~3;
    int dataSize = rowSize * height;

    unsigned char fileHeader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
    unsigned char infoHeader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};

    int fileSize = 54 + dataSize;
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);

    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);

    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);

    std::ofstream f(filename, std::ios::binary);
    if (!f) {
        return;
    }

    f.write((char *)fileHeader, 14);
    f.write((char *)infoHeader, 40);

    std::vector<unsigned char> row(rowSize, 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const float *p = pixelData + (y * size.width + x) * 3;

            row[x * 3 + 0] = (unsigned char)msdfgen::pixelFloatToByte(p[2]);
            row[x * 3 + 1] = (unsigned char)msdfgen::pixelFloatToByte(p[1]);
            row[x * 3 + 2] = (unsigned char)msdfgen::pixelFloatToByte(p[0]);
        }
        f.write((char *)row.data(), rowSize);
    }
    f.close();
}

void saveToBmp(const msdfgen::Bitmap<float, 3> &bitmap, const char *filename) {
    saveToBmp(bitmap(0, 0), Size{(Size::Type)bitmap.width(), (Size::Type)bitmap.height()},
              filename);
}
} // namespace rendell_text