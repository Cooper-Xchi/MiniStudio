#pragma once



class Texture2D {
public:
    Texture2D() = default;
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    bool Initialize(
        int width,
        int height,
        const unsigned char* rgba_pixels
    );

    void Bind(unsigned int texture_unit) const;

private:
    void Release();
    unsigned int texture_id_ = 0;
};
