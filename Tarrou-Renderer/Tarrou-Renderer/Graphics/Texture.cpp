//
//  Texture.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/15/26.
//

#include "Texture.hpp"
#include "MetalTextureLoader.h"

Texture::Texture(
    const std::string& path,
    void* nativeTexture,
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels)
    : m_Path(path)
    , m_NativeTexture(nativeTexture)
    , m_Width(width)
    , m_Height(height)
    , m_MipLevels(mipLevels) {
} // Texture

Texture::~Texture() {
    if (m_NativeTexture) {
        MetalTextureLoader::ReleaseTexture(
            m_NativeTexture
        );

        m_NativeTexture = nullptr;
    }
} // ~Texture

Texture::Texture(Texture&& other) noexcept
    : m_Path(std::move(other.m_Path))
    , m_NativeTexture(other.m_NativeTexture)
    , m_Width(other.m_Width)
    , m_Height(other.m_Height)
    , m_MipLevels(other.m_MipLevels) {
    other.m_NativeTexture = nullptr;
    other.m_Width = 0;
    other.m_Height = 0;
    other.m_MipLevels = 1;
} // Texture

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    if (m_NativeTexture) {
        MetalTextureLoader::ReleaseTexture(
            m_NativeTexture
        );
    }

    m_Path = std::move(other.m_Path);

    m_NativeTexture = other.m_NativeTexture;

    m_Width = other.m_Width;
    m_Height = other.m_Height;
    m_MipLevels = other.m_MipLevels;

    other.m_NativeTexture = nullptr;
    other.m_Width = 0;
    other.m_Height = 0;
    other.m_MipLevels = 1;

    return *this;
} // Texture

uint32_t           Texture::GetWidth() const { return m_Width; }
uint32_t           Texture::GetHeight() const { return m_Height; }
uint32_t           Texture::GetMipLevels() const { return m_MipLevels; }
const std::string& Texture::GetPath() const { return m_Path; }
void*              Texture::GetNativeTexture() const { return m_NativeTexture; }
