//
//  RendererBridge.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#import <Metal/Metal.h>
#import "RendererBridge.h"
#import "MeshData.hpp"

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

extern "C" bool RendererBridge_InitPipeline(void* devicePtr,
                                             const char* vertexFunctionName,
                                             const char* fragmentFunctionName,
                                             void** outPipelineState) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        if (!device || !vertexFunctionName || !fragmentFunctionName) return false;

        id<MTLLibrary> defaultLibrary = [device newDefaultLibrary];
        if (!defaultLibrary) {
            NSLog(@"[RendererBridge] Default shader library not found!");
            return false;
        }

        NSString *vertexName = [NSString stringWithUTF8String:vertexFunctionName];
        NSString *fragmentName = [NSString stringWithUTF8String:fragmentFunctionName];

        id<MTLFunction> vertexFunc = [defaultLibrary newFunctionWithName:vertexName];
        id<MTLFunction> fragmentFunc = [defaultLibrary newFunctionWithName:fragmentName];

        if (!vertexFunc || !fragmentFunc) {
            NSLog(@"[RendererBridge] 함수를 찾지 못함: vertex=%@ fragment=%@", vertexName, fragmentName);
            return false;
        }

        MTLRenderPipelineDescriptor *pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDesc.vertexFunction = vertexFunc;
        pipelineDesc.fragmentFunction = fragmentFunc;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

        NSError* error = nil;
        id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
        if (!pipelineState) {
            NSLog(@"[RendererBridge] Pipeline creation error: %@", error);
            return false;
        }

        *outPipelineState = (void*)CFBridgingRetain(pipelineState);
        return true;
    }
} // RendererBridge_InitPipeline

extern "C" void RendererBridge_DrawMesh(void* encoderPtr, void* pipelineStatePtr, void* depthStatePtr,
                                        const void* meshPtr, simd_float4x4 viewProjMatrix) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)encoderPtr;
    id<MTLRenderPipelineState> pipelineState = (__bridge id<MTLRenderPipelineState>)pipelineStatePtr;
    id<MTLDepthStencilState> depthState = (__bridge id<MTLDepthStencilState>)depthStatePtr;
    const Mesh* mesh = static_cast<const Mesh*>(meshPtr);

    if (!encoder || !pipelineState || !mesh) return;

    [encoder setRenderPipelineState:pipelineState];
    if (depthState) {
        [encoder setDepthStencilState:depthState];
    }

    [encoder setVertexBytes:&viewProjMatrix length:sizeof(simd_float4x4) atIndex:1];

    for (const auto& part : mesh->parts) {
        id<MTLBuffer> vertexBuffer = (__bridge id<MTLBuffer>)part.vertexBuffer.Get();
        [encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];

        for (const auto& subMesh : part.subMeshes) {
            id<MTLBuffer> indexBuffer = (__bridge id<MTLBuffer>)subMesh.indexBuffer.Get();
            MTLIndexType indexType = (subMesh.indexTypeBytes == 4) ? MTLIndexTypeUInt32 : MTLIndexTypeUInt16;

            [encoder drawIndexedPrimitives:(MTLPrimitiveType)subMesh.primitiveType
                                indexCount:subMesh.indexCount
                                 indexType:indexType
                               indexBuffer:indexBuffer
                         indexBufferOffset:0];
        } // for (const auto& subMesh : part.subMeshes)
    } // for (const auto& part : mesh->parts)
    
} // RendererBridge_DrawMesh
