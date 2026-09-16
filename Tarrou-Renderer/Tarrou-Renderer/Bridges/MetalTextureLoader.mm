//
//  MetalTextureLoader.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/15/26.
//

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include "MetalTextureLoader.h"

NativeTexture MetalTextureLoader::Load(void* device, const std::string& path) {
    NativeTexture result;

    if (device == nullptr) {
        NSLog(@"[Tarrou] Metal device is null.");
        return result;
    }

    id<MTLDevice> metalDevice = (__bridge id<MTLDevice>)device;

    NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];

    if (nsPath == nil) {
        NSLog(@"[Tarrou] Invalid texture path: %s", path.c_str());
        return result;
    }

    NSURL* url = [NSURL fileURLWithPath:nsPath];

    MTKTextureLoader* loader = [[MTKTextureLoader alloc] initWithDevice:metalDevice];
    NSError* error = nil;

    NSDictionary* options =
    @{
        MTKTextureLoaderOptionGenerateMipmaps : @YES,
        MTKTextureLoaderOptionSRGB : @NO
    };

    id<MTLTexture> texture = [loader newTextureWithContentsOfURL:url
                                                         options:options
                                                           error:&error];

    if (texture == nil) {
        NSLog(@"[Tarrou] Failed to load texture: %@", error.localizedDescription);
        return result;
    }

    result.texture = (__bridge_retained void*)texture; // MetalResource::Adopt가 소유권을 넘겨받음
    result.width = static_cast<uint32_t>(texture.width);
    result.height = static_cast<uint32_t>(texture.height);
    result.mipLevels = static_cast<uint32_t>(texture.mipmapLevelCount);

    return result;
} // Load
