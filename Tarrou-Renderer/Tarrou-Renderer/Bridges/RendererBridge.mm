//
//  RendererBridge.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#import <Metal/Metal.h>
#import "RendererBridge.h"
#import "MeshData.hpp"
#import "ShaderLoader.hpp"

extern "C" void* RendererBridge_CreateDepthState(void* devicePtr) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        if (!device) return nullptr;

        MTLDepthStencilDescriptor *depthDesc = [[MTLDepthStencilDescriptor alloc] init];
        depthDesc.depthCompareFunction = MTLCompareFunctionGreaterEqual; // Reverse-Z
        depthDesc.depthWriteEnabled = YES;
        id<MTLDepthStencilState> depthState = [device newDepthStencilStateWithDescriptor:depthDesc];
        if (!depthState) return nullptr;

        return (void*)CFBridgingRetain(depthState);
    }
} // RendererBridge_CreateDepthState

extern "C" void* RendererBridge_GetBufferContents(void* mtlBuffer) {
    if (!mtlBuffer) return nullptr;
    id<MTLBuffer> buffer = (__bridge id<MTLBuffer>)mtlBuffer;
    return [buffer contents];
} // RendererBridge_GetBufferContents

extern "C" void* RendererBridge_CreateBuffer(void* devicePtr, const void* data, size_t length) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        if (!device || !data || length == 0) return nullptr;

        id<MTLBuffer> buffer = [device newBufferWithBytes:data
                                       length:length
                                       options:MTLResourceStorageModeShared];
        
        return (void*)CFBridgingRetain(buffer);
    }
} // RendererBridge_CreateBuffer

extern "C" void* RendererBridge_CreateShadowSampler(void* devicePtr) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        if (!device) return nullptr;

        MTLSamplerDescriptor *desc = [[MTLSamplerDescriptor alloc] init];
        desc.minFilter = MTLSamplerMinMagFilterNearest;
        desc.magFilter = MTLSamplerMinMagFilterNearest;
        desc.sAddressMode = MTLSamplerAddressModeClampToEdge;
        desc.tAddressMode = MTLSamplerAddressModeClampToEdge;

        id<MTLSamplerState> sampler = [device newSamplerStateWithDescriptor:desc];
        return (void*)CFBridgingRetain(sampler);
    }
} // RendererBridge_CreateShadowSampler

extern "C" void RendererBridge_BindConstantBuffers(void*       encoderPtr,
                                                   const void* frameCBPtr, size_t frameCBSize,
                                                   const void* lightCBPtr, size_t lightCBSize,
                                                   void*       shadowTexturePtr,
                                                   void*       shadowSamplerPtr) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)encoderPtr;
    if (!encoder) return;

    [encoder setVertexBytes:frameCBPtr length:frameCBSize atIndex:1];
    [encoder setFragmentBytes:frameCBPtr length:frameCBSize atIndex:1];
    [encoder setMeshBytes:frameCBPtr length:frameCBSize atIndex:1];
            
    [encoder setVertexBytes:lightCBPtr length:lightCBSize atIndex:2];
    [encoder setFragmentBytes:lightCBPtr length:lightCBSize atIndex:2];
    [encoder setMeshBytes:lightCBPtr length:lightCBSize atIndex:2];

    // Texture Slot 1: ShadowMap, Sampler Slot 1: ShadowSampler (공용 바인딩)
    if (shadowTexturePtr) {
        id<MTLTexture> shadowTex = (__bridge id<MTLTexture>)shadowTexturePtr;
        [encoder setFragmentTexture:shadowTex atIndex:1];
    }
    if (shadowSamplerPtr) {
        id<MTLSamplerState> shadowSamp = (__bridge id<MTLSamplerState>)shadowSamplerPtr;
        [encoder setFragmentSamplerState:shadowSamp atIndex:1];
    }
} // RendererBridge_BindConstantBuffers

extern "C" void* RendererBridge_BeginRenderPass(void* commandBufferPtr, void* passDescriptorPtr) {
    id<MTLCommandBuffer> commandBuffer = (__bridge id<MTLCommandBuffer>)commandBufferPtr;
    MTLRenderPassDescriptor* passDesc = (__bridge MTLRenderPassDescriptor*)passDescriptorPtr;
    
    id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:passDesc];
    return (__bridge void*)encoder;
} // RendererBridge_BeginRenderPass

extern "C" void RendererBridge_EndEncoding(void* encoderPtr) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)encoderPtr;
    [encoder endEncoding];
} // RendererBridge_EndEncoding
