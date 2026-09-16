//
//  Texture.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/15/26.
//

#include "Texture.hpp"

Texture::Texture(
    const std::string& path,
    MetalResource      nativeTexture,
    uint32_t           width,
    uint32_t           height,
    uint32_t           mipLevels)
    : m_Path(path)
    , m_NativeTexture(std::move(nativeTexture))
    , m_Width(width)
    , m_Height(height)
    , m_MipLevels(mipLevels) {
} // Texture

uint32_t           Texture::GetWidth() const { return m_Width; }
uint32_t           Texture::GetHeight() const { return m_Height; }
uint32_t           Texture::GetMipLevels() const { return m_MipLevels; }
const std::string& Texture::GetPath() const { return m_Path; }
void*              Texture::GetNativeTexture() const { return m_NativeTexture.Get(); }
