#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

ImageUPtr Image::Load(const std::string& filepath, bool defaultPath)
{
    std::string fullPath = defaultPath ? std::string(IMAGE_PATH) : "";
    fullPath += filepath;

    ImageUPtr image(new Image());
    if (!image->LoadWithStb(fullPath))
    {
        return nullptr;
    }
    return std::move(image);
}

ImageUPtr Image::CreateSingleColorImage(int width, int height, const glm::vec4& color)
{
    glm::vec4 clamped = glm::clamp(color*255.0f, 0.0f, 255.0f);
    uint8_t rgba[4] = {
        static_cast<uint8_t>(clamped.r),
        static_cast<uint8_t>(clamped.g),
        static_cast<uint8_t>(clamped.b),
        static_cast<uint8_t>(clamped.a),
    };

    auto image = Create(width, height, 4);
    for (int i=0; i<width*height; ++i)
    {
        memcpy(image->m_data + i*4, rgba, 4);
    }
    
    return std::move(image);
}

Image::~Image() {
    if (m_data)
    {
        stbi_image_free(m_data);
    }
}

bool Image::LoadWithStb(const std::string& filepath) {
   
    stbi_set_flip_vertically_on_load(true);
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data) {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}

ImageUPtr Image::Create(int width, int height, int channelCount) {
    ImageUPtr image(new Image());
    if (!image->Allocate(width, height, channelCount)) {
        return nullptr;
    }
    return std::move(image);
}

bool Image::Allocate(int width, int height, int channelCount) {
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;

    // 소멸자에서 stbi_image_free 함수를 호출하고 있기 때문에 delete[] 가 아닌 free() 가 불림
    // new uint_8t[...] 를 사용하면 안됨
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}

void Image::SetCheckImage(int gridX, int gridY) {
    for (int i = 0; i < m_height; ++i)
    {
        for (int j = 0; j < m_width; ++j)
        {
            int pos = (j*m_width + i) * m_channelCount;
            bool even = ((i / gridY) + (j / gridX)) % 2 == 0;
            uint8_t color = even ? 255 : 0;

            for (int k = 0; k < m_channelCount; ++k)
            {
                m_data[pos + k] = color;
            }
            if (m_channelCount > 3)
            {
                m_data[pos + 3] = 255;
            }
        }
    }
}