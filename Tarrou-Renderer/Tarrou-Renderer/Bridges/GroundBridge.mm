//
//  GroundBridge.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#import <Metal/Metal.h>
#import "GroundBridge.h"
 
extern "C" bool GroundBridge_InitPipeline(void*       devicePtr,
                                          const char* shaderSourceRaw,
                                          const char* vertexFunctionName,
                                          const char* fragmentFunctionName,
                                          void**      outPipelineState,
                                          void**      outSamplerState) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        NSString* shaderSource = [NSString stringWithUTF8String:shaderSourceRaw];
        NSError* error = nil;

        id<MTLLibrary> customLibrary = [device newLibraryWithSource:shaderSource options:nil error:&error];
        if (!customLibrary) {
            NSLog(@"[RendererBridge] Shader Compile Error: %@", error);
            return false;
        }
        
        NSString *vertexName = [NSString stringWithUTF8String:vertexFunctionName];
        NSString *fragmentName = [NSString stringWithUTF8String:fragmentFunctionName];

        id<MTLFunction> vertexFunc = [customLibrary newFunctionWithName:vertexName];
        id<MTLFunction> fragmentFunc = [customLibrary newFunctionWithName:fragmentName];

        if (!vertexFunc || !fragmentFunc) {
            NSLog(@"[RendererBridge] 함수를 찾지 못함: vertex=%@ fragment=%@", vertexName, fragmentName);
            return false;
        }
 
        MTLRenderPipelineDescriptor *pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDesc.label = @"GroundPipeline";
        pipelineDesc.vertexFunction = vertexFunc;
        pipelineDesc.fragmentFunction = fragmentFunc;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        
        id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
        if (!pipelineState) {
            NSLog(@"[GroundBridge] Pipeline creation error: %@", error);
            return false;
        }
 
        MTLSamplerDescriptor *samplerDesc = [[MTLSamplerDescriptor alloc] init];
        samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.mipFilter = MTLSamplerMipFilterLinear;
        samplerDesc.sAddressMode = MTLSamplerAddressModeRepeat;
        samplerDesc.tAddressMode = MTLSamplerAddressModeRepeat;
        id<MTLSamplerState> samplerState = [device newSamplerStateWithDescriptor:samplerDesc];
        if (!samplerState) {
            NSLog(@"[GroundBridge] Sampler state creation failed");
            return false;
        }
 
        *outPipelineState = (void*)CFBridgingRetain(pipelineState);
        *outSamplerState = (void*)CFBridgingRetain(samplerState);
        return true;
    }
} // GroundBridge_InitPipeline
 
extern "C" void* GroundBridge_CreateVertexBuffer(void*         devicePtr,
                                                 const void*   vertexData,
                                                 unsigned long byteLength) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        if (!device || !vertexData || byteLength == 0) return nullptr;
 
        id<MTLBuffer> buffer = [device newBufferWithBytes:vertexData
                                                     length:byteLength
                                                    options:MTLResourceStorageModeShared];
        if (!buffer) return nullptr;
 
        return (void*)CFBridgingRetain(buffer);
    }
} // GroundBridge_CreateVertexBuffer
 
extern "C" void GroundBridge_Draw(void*        encoderPtr,
                                  void*        pipelineStatePtr,
                                  void*        depthStatePtr,
                                  void*        vertexBufferPtr,
                                  unsigned int vertexCount,
                                  void*        texturePtr,
                                  void*        samplerStatePtr) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)encoderPtr;
    id<MTLRenderPipelineState> pipelineState = (__bridge id<MTLRenderPipelineState>)pipelineStatePtr;
    id<MTLDepthStencilState> depthState = (__bridge id<MTLDepthStencilState>)depthStatePtr;
    id<MTLBuffer> vertexBuffer = (__bridge id<MTLBuffer>)vertexBufferPtr;
    id<MTLTexture> texture = (__bridge id<MTLTexture>)texturePtr;
    id<MTLSamplerState> samplerState = (__bridge id<MTLSamplerState>)samplerStatePtr;
 
    if (!encoder || !pipelineState || !vertexBuffer || !texture) return;
 
    [encoder setRenderPipelineState:pipelineState];
    if (depthState) {
        [encoder setDepthStencilState:depthState];
    }
    [encoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [encoder setCullMode:MTLCullModeBack];
 
    [encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];
 
    [encoder setFragmentTexture:texture atIndex:0];
    [encoder setFragmentSamplerState:samplerState atIndex:0];
 
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:vertexCount];
} // GroundBridge_Draw

