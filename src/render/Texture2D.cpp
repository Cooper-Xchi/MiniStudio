#include "opengl/OpenGLHeaders.h"
#include "Texture2D.h"

#include <utility>

bool Texture2D::Initialize(
        int width,
        int height,
        const unsigned char* rgba_pixels
    ) {
    if (width <= 0 ||
        height <= 0||
        rgba_pixels == nullptr||
        texture_id_ != 0
        ) {
        return false;
    }
    glGenTextures(1,&texture_id_);
    if (texture_id_ == 0) {
        return false;
    }
    glBindTexture(GL_TEXTURE_2D,texture_id_);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        rgba_pixels);




    return true;

}

void Texture2D::Bind(unsigned int texture_unit) const {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D,texture_id_);
}

void Texture2D::Release() {
    if (texture_id_ != 0) {
        glDeleteTextures(1,&texture_id_);
    }
    texture_id_ = 0;
}

Texture2D::~Texture2D() {
    Release();
}

Texture2D::Texture2D(Texture2D&& other)noexcept:texture_id_(other.texture_id_) {
    other.texture_id_ = 0;
}
Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    Release();
    texture_id_ = std::exchange(other.texture_id_,0);



    return *this;
}
