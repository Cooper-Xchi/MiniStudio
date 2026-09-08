#include "image/ImageLoader.h"
#include <cstddef>
#include <memory>
#include <utility>
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
    output = std::move(decode);

    return true;
}


