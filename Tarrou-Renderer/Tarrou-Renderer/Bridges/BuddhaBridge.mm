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

extern "C" void BuddhaBridge_DrawMeshletsInctance(void*    encoderPtr,
                                                  void*    pipelineStatePtr,
                                                  void*    depthStatePtr,
                                                  void*    meshletBuffer,
                                                  void*    meshletVerticesBuffer,
                                                  void*    meshletTrianglesBuffer,
                                                  void*    vertexBuffer,
                                                  size_t   vertexBufferOffset,
                                                  void*    instanceBuffer,
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
     
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletBuffer offset:0 atIndex:0];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletVerticesBuffer offset:0 atIndex:3];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletTrianglesBuffer offset:0 atIndex:4];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)vertexBuffer offset:vertexBufferOffset atIndex:5];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)instanceBuffer offset:0 atIndex:6];
     
    MTLSize threadgroupsPerGrid = MTLSizeMake(meshletCount, instanceCount, 1);
    MTLSize threadsPerObjectThreadgroup = MTLSizeMake(1, 1, 1);
    MTLSize threadsPerMeshThreadgroup = MTLSizeMake(128, 1, 1);
     
    [encoder drawMeshThreadgroups:threadgroupsPerGrid
        threadsPerObjectThreadgroup:threadsPerObjectThreadgroup
        threadsPerMeshThreadgroup:threadsPerMeshThreadgroup];
} // BuddhaBridge_DrawMeshletsInctance
