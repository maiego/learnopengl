#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)

class Texture
{
public:
    static TextureUPtr CreateFromImage(const Image* image);
    static TextureUPtr Create(int width, int height, uint32_t format);
    ~Texture();

    const uint32_t Get() const { return m_texture; }
    const int GetWidth() const { return m_width; }
    const int GetHeight() const { return m_height; }

    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
    void SetTextureFormat(int width, int height, uint32_t format);

private:
    Texture() {}
    void CreateTexture();
    void CreateTextureFromImage(const Image* image);

    uint32_t m_texture { 0 };
    int m_width { 0 };
    int m_height { 0 };
    uint32_t m_format { 0 };
};

#endif // __TEXTURE_H__