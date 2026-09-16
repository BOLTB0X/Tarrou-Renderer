//
//  TextureLoader.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/15/26.
//

#include "TextureLoader.hpp"
#include "Texture.hpp"
#include "MetalTextureLoader.h"
#include "MetalResource.h"

TextureLoader::TextureLoader(void* device)
    : m_Device(device) {
} // TextureLoader

std::shared_ptr<Texture> TextureLoader::Load(const std::string& path) {
    NativeTexture native = MetalTextureLoader::Load(m_Device, path);
    if (!native.texture) {
        return nullptr;
    }

    return std::shared_ptr<Texture>(
        new Texture(path, MetalResource::Adopt(native.texture), native.width, native.height, native.mipLevels)
    );
} // Load
