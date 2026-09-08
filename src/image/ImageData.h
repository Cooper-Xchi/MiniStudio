#pragma once

#include <vector>

struct ImageData {
    int width = 0;
    int height = 0;
    std::vector<unsigned char> rgba_pixels;
};
