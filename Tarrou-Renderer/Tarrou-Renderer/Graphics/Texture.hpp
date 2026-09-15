//
//  Texture.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/15/26.
//

#ifndef Texture_hpp
#define Texture_hpp

#include <stdio.h>
#include <cstdint>
#include <string>

class Texture {
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    uint32_t           GetWidth() const;
    uint32_t           GetHeight() const;
    uint32_t           GetMipLevels() const;
    const std::string& GetPath() const;
    void*              GetNativeTexture() const;

private:
    friend class TextureLoader;

    Texture(const std::string& path,
            void* nativeTexture,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels
    );

private:
    std::string m_Path;
    void*       m_NativeTexture = nullptr;
    uint32_t    m_Width = 0;
    uint32_t    m_Height = 0;
    uint32_t    m_MipLevels = 1;
}; // Texture

#endif /* Texture_hpp */
