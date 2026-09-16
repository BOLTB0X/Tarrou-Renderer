//
//  MetalTextureLoader.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/15/26.
//

#include <cstdint>
#include <string>

struct NativeTexture {
    void*    texture = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipLevels = 1;
}; // NativeTexture

class MetalTextureLoader {
public:
    static NativeTexture Load(void* device, const std::string& path);
}; // MetalTextureLoader
