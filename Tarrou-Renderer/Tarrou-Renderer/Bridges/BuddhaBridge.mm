//
//  BuddhaBridge.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#import <Metal/Metal.h>
#import "BuddhaBridge.h"
#import "MeshData.hpp"
#import "ShaderLoader.hpp"

extern "C" bool BuddhaBridge_InitMeshPipeline(void*       devicePtr,
                                              const char* shaderSourceRaw,
                                              const char* meshFunctionName,
                                              const char* fragmentFunctionName,
                                              void**      outPipelineState) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        NSString* shaderSource = [NSString stringWithUTF8String:shaderSourceRaw];
        NSError* error = nil;

        id<MTLLibrary> customLibrary = [device newLibraryWithSource:shaderSource options:nil error:&error];
        if (!customLibrary) {
            NSLog(@"[RendererBridge] Shader Compile Error: %@", error);
            return false;
        }

        NSString* meshName = [NSString stringWithUTF8String:meshFunctionName];
        NSString* fragmentName = [NSString stringWithUTF8String:fragmentFunctionName];

        id<MTLFunction> meshFunc = [customLibrary newFunctionWithName:meshName];
        id<MTLFunction> fragmentFunc = [customLibrary newFunctionWithName:fragmentName];

        if (!meshFunc || !fragmentFunc) {
            NSLog(@"[RendererBridge] 함수를 찾지 못함: mesh=%@ fragment=%@", meshName, fragmentName);
            return false;
        }

        MTLMeshRenderPipelineDescriptor* pipelineDesc = [[MTLMeshRenderPipelineDescriptor alloc] init];
        pipelineDesc.meshFunction = meshFunc;
        pipelineDesc.fragmentFunction = fragmentFunc;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        
        id<MTLRenderPipelineState> pipelineState =
            [device newRenderPipelineStateWithMeshDescriptor:pipelineDesc options:0 reflection:nil error:&error];

        if (!pipelineState) {
            NSLog(@"[RendererBridge] Mesh Pipeline creation error: %@", error);
            return false;
        }

        *outPipelineState = (void*)CFBridgingRetain(pipelineState);
        return true;
    }
} // RendererBridge_InitMeshPipeline

extern "C" void BuddhaBridge_DrawMeshlets(void*  encoderPtr,
                                          void*  pipelineStatePtr,
                                          void*  depthStatePtr,
                                          void*  meshletBuffer,
                                          void*  meshletVerticesBuffer,
                                          void*  meshletTrianglesBuffer,
                                          void*  vertexBuffer,
                                          size_t meshletCount,
                                          simd_float4x4 modelMatrix) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)encoderPtr;
    id<MTLRenderPipelineState> pipelineState = (__bridge id<MTLRenderPipelineState>)pipelineStatePtr;
    id<MTLDepthStencilState> depthState = (__bridge id<MTLDepthStencilState>)depthStatePtr;

    if (!encoder || !pipelineState) return;

    [encoder setRenderPipelineState:pipelineState];
    if (depthState) {
        [encoder setDepthStencilState:depthState];
    }
    [encoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [encoder setCullMode:MTLCullModeBack];

    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletBuffer offset:0 atIndex:0];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletVerticesBuffer offset:0 atIndex:3];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletTrianglesBuffer offset:0 atIndex:4];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)vertexBuffer offset:0 atIndex:5];
    [encoder setMeshBytes:&modelMatrix length:sizeof(simd_float4x4) atIndex:6];

    MTLSize threadgroupsPerGrid = MTLSizeMake(meshletCount, 1, 1);
    MTLSize threadsPerThreadgroup = MTLSizeMake(128, 1, 1);

    [encoder drawMeshThreadgroups:threadgroupsPerGrid
      threadsPerObjectThreadgroup:MTLSizeMake(1, 1, 1)
        threadsPerMeshThreadgroup:threadsPerThreadgroup];
} // BuddhaBridge_DrawMeshlets

extern "C" bool BuddhaBridge_InitMeshletsInctancePipeline(void*       devicePtr,
                                                          const char* shaderSourceRaw,
                                                          const char* objectFunctionName,
                                                          const char* meshFunctionName,
                                                          const char* fragmentFunctionName,
                                                          size_t      payloadSize,
                                                          void**      outPipelineState) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        NSString* shaderSource = [NSString stringWithUTF8String:shaderSourceRaw];
        NSError* error = nil;
     
        id<MTLLibrary> customLibrary = [device newLibraryWithSource:shaderSource options:nil error:&error];
        if (!customLibrary) {
            NSLog(@"[BuddhaBridge] Shader Compile Error: %@", error);
            return false;
        }
     
        NSString* objectName = [NSString stringWithUTF8String:objectFunctionName];
        NSString* meshName = [NSString stringWithUTF8String:meshFunctionName];
        NSString* fragmentName = [NSString stringWithUTF8String:fragmentFunctionName];
     
        id<MTLFunction> objectFunc = [customLibrary newFunctionWithName:objectName];
        id<MTLFunction> meshFunc = [customLibrary newFunctionWithName:meshName];
        id<MTLFunction> fragmentFunc = [customLibrary newFunctionWithName:fragmentName];
     
        if (!objectFunc || !meshFunc || !fragmentFunc) {
            NSLog(@"[BuddhaBridge] 함수를 찾지 못함: object=%@ mesh=%@ fragment=%@", objectName, meshName, fragmentName);
                return false;
        }
     
        MTLMeshRenderPipelineDescriptor* pipelineDesc = [[MTLMeshRenderPipelineDescriptor alloc] init];
        pipelineDesc.objectFunction = objectFunc;
        pipelineDesc.meshFunction = meshFunc;
        pipelineDesc.fragmentFunction = fragmentFunc;
        pipelineDesc.payloadMemoryLength = payloadSize;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
     
        id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithMeshDescriptor:pipelineDesc options:0 reflection:nil error:&error];
     
        if (!pipelineState) {
            NSLog(@"[BuddhaBridge] Mesh Pipeline creation error: %@", error);
            return false;
        }
     
        *outPipelineState = (void*)CFBridgingRetain(pipelineState);
        return true;
    }
} // BuddhaBridge_InitMeshletsInctancePipeline

extern "C" bool BuddhaBridge_InitConeDebugPipeline(void* devicePtr,
                                                    const char* shaderSourceRaw,
                                                    const char* vertexFunctionName,
                                                    const char* fragmentFunctionName,
                                                    void** outPipelineState,
                                                    void** outDepthState) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        NSString* shaderSource = [NSString stringWithUTF8String:shaderSourceRaw];
        NSError* error = nil;
        id<MTLLibrary> library = [device newLibraryWithSource:shaderSource options:nil error:&error];
        if (!library) {
            NSLog(@"[BuddhaBridge] Cone debug shader compile error: %@", error);
            return false;
        }

        id<MTLFunction> vertexFunction = [library newFunctionWithName:[NSString stringWithUTF8String:vertexFunctionName]];
        id<MTLFunction> fragmentFunction = [library newFunctionWithName:[NSString stringWithUTF8String:fragmentFunctionName]];
        if (!vertexFunction || !fragmentFunction) return false;

        MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
        descriptor.label = @"NormalConeDebugPipeline";
        descriptor.vertexFunction = vertexFunction;
        descriptor.fragmentFunction = fragmentFunction;
        descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        descriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        descriptor.colorAttachments[0].blendingEnabled = YES;
        descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
        descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

        id<MTLRenderPipelineState> pipeline = [device newRenderPipelineStateWithDescriptor:descriptor error:&error];
        if (!pipeline) {
            NSLog(@"[BuddhaBridge] Cone debug pipeline creation error: %@", error);
            return false;
        }

        MTLDepthStencilDescriptor* depthDescriptor = [[MTLDepthStencilDescriptor alloc] init];
        depthDescriptor.depthCompareFunction = MTLCompareFunctionGreaterEqual;
        depthDescriptor.depthWriteEnabled = NO;
        id<MTLDepthStencilState> depthState = [device newDepthStencilStateWithDescriptor:depthDescriptor];
        if (!depthState) return false;

        *outPipelineState = (void*)CFBridgingRetain(pipeline);
        *outDepthState = (void*)CFBridgingRetain(depthState);
        return true;
    }
}

extern "C" void BuddhaBridge_DrawConeDebug(void* encoderPtr,
                                           void* pipelineStatePtr,
                                           void* depthStatePtr,
                                           void* vertexBufferPtr,
                                           uint32_t vertexStart,
                                           uint32_t vertexCount,
                                           void* instanceBufferPtr,
                                           uint32_t instanceCount) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)encoderPtr;
    id<MTLRenderPipelineState> pipeline = (__bridge id<MTLRenderPipelineState>)pipelineStatePtr;
    id<MTLDepthStencilState> depthState = (__bridge id<MTLDepthStencilState>)depthStatePtr;
    if (!encoder || !pipeline || !vertexBufferPtr || !instanceBufferPtr || vertexCount == 0 || instanceCount == 0) return;

    [encoder setRenderPipelineState:pipeline];
    [encoder setDepthStencilState:depthState];
    [encoder setCullMode:MTLCullModeNone];
    [encoder setVertexBuffer:(__bridge id<MTLBuffer>)vertexBufferPtr offset:0 atIndex:0];
    [encoder setVertexBuffer:(__bridge id<MTLBuffer>)instanceBufferPtr offset:0 atIndex:6];
    [encoder drawPrimitives:MTLPrimitiveTypeLine
                vertexStart:vertexStart
                vertexCount:vertexCount
              instanceCount:instanceCount];
}

extern "C" void BuddhaBridge_DrawMeshletsInctance(void*    encoderPtr,
                                                  void*    pipelineStatePtr,
                                                  void*    depthStatePtr,
                                                  void*    meshletBuffer,
                                                  void*    meshletVerticesBuffer,
                                                  void*    meshletTrianglesBuffer,
                                                  void*    meshletBoundsBuffer,
                                                  void*    vertexBuffer,
                                                  size_t   vertexBufferOffset,
                                                  void*    instanceBuffer,
                                                  const simd_float4* frustumPlanes,
                                                  simd_float4 cameraPositionAndCulling,
                                                  size_t   meshletCount,
                                                  uint32_t instanceCount) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)encoderPtr;
    id<MTLRenderPipelineState> pipelineState = (__bridge id<MTLRenderPipelineState>)pipelineStatePtr;
    id<MTLDepthStencilState> depthState = (__bridge id<MTLDepthStencilState>)depthStatePtr;
     
    if (!encoder || !pipelineState || meshletCount == 0 || instanceCount == 0) return;
     
    [encoder setRenderPipelineState:pipelineState];
    if (depthState) {
        [encoder setDepthStencilState:depthState];
    }
    [encoder setFrontFacingWinding:MTLWindingClockwise];
    [encoder setCullMode:MTLCullModeBack];
     
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletBuffer offset:0 atIndex:0];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletVerticesBuffer offset:0 atIndex:3];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletTrianglesBuffer offset:0 atIndex:4];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)vertexBuffer offset:vertexBufferOffset atIndex:5];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)instanceBuffer offset:0 atIndex:6];
    if (frustumPlanes && meshletBoundsBuffer) {
        [encoder setObjectBytes:frustumPlanes length:sizeof(simd_float4) * 6 atIndex:1];
        [encoder setObjectBuffer:(__bridge id<MTLBuffer>)meshletBoundsBuffer offset:0 atIndex:7];
        [encoder setObjectBuffer:(__bridge id<MTLBuffer>)instanceBuffer offset:0 atIndex:6];
        [encoder setObjectBytes:&cameraPositionAndCulling length:sizeof(simd_float4) atIndex:8];
    }
     
    MTLSize threadgroupsPerGrid = MTLSizeMake(meshletCount, instanceCount, 1);
    MTLSize threadsPerObjectThreadgroup = MTLSizeMake(1, 1, 1);
    MTLSize threadsPerMeshThreadgroup = MTLSizeMake(128, 1, 1);
     
    [encoder drawMeshThreadgroups:threadgroupsPerGrid
        threadsPerObjectThreadgroup:threadsPerObjectThreadgroup
        threadsPerMeshThreadgroup:threadsPerMeshThreadgroup];
} // BuddhaBridge_DrawMeshletsInctance

