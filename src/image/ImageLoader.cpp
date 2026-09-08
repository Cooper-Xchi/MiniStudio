#include "image/ImageLoader.h"
#include <cstddef>
#include <memory>
#include <utility>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>

using StbiPixels =
    std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>;

bool LoadImageRgba(const char* path, ImageData& output) {
    if (path == nullptr || path[0] == '\0' ) return false;
    int width=0, height=0, channels=0;
    StbiPixels imagePt(
        stbi_load(path, &width, &height, &channels, STBI_rgb_alpha)
        ,&stbi_image_free
    );
    if (!imagePt) {
        const char* reason = stbi_failure_reason();
        std::cerr
            << "Failed to load image: "
            << path
            << " ("
            << (reason != nullptr ? reason : "unknown error")
            << ")\n";
        return false;
    }
    if(width <= 0 || height <= 0){
        std::cerr<<"width or height Failed!"<<std::endl;
        return false;
    }
    ImageData decode;
    const std::size_t byte_count =
    static_cast<std::size_t>(width) *
    static_cast<std::size_t>(height) *
    STBI_rgb_alpha;
    decode.width = width;
    decode.height = height;
    decode.rgba_pixels.assign(
        imagePt.get(),
        imagePt.get() + byte_count
    );
    std::size_t row_bytes = static_cast<std::size_t>(width)* STBI_rgb_alpha;
    std::size_t row_count = static_cast<std::size_t>(height);
    for (std::size_t row = 0; row < row_count/2; ++row) {
        std::size_t mirror_row = row_count - row - 1;
        auto top_begin =
            decode.rgba_pixels.begin() + row * row_bytes;
        auto bottom_begin =
            decode.rgba_pixels.begin() + mirror_row * row_bytes;

        std::swap_ranges(
            top_begin,
            top_begin + row_bytes,
            bottom_begin
        );
    }

    output = std::move(decode);

    return true;
}


