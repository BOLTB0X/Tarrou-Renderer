//
//  RendererBridge.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#import <Metal/Metal.h>
#import "RendererBridge.h"
#import "MeshData.hpp"

extern "C" bool RendererBridge_InitPipeline(void* devicePtr, void** outPipelineState, void** outDepthState) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        if (!device) return false;

        // Depth Stencil State 생성
        MTLDepthStencilDescriptor *depthDesc = [[MTLDepthStencilDescriptor alloc] init];
        depthDesc.depthCompareFunction = MTLCompareFunctionGreaterEqual;
        depthDesc.depthWriteEnabled = YES;
        id<MTLDepthStencilState> depthState = [device newDepthStencilStateWithDescriptor:depthDesc];
        
        // C++에서 보관할 수 있도록 Retain 처리하여 void*로 넘김
        *outDepthState = (void*)CFBridgingRetain(depthState);

        // 기본 Shader Library 로드
        id<MTLLibrary> defaultLibrary = [device newDefaultLibrary];
        if (!defaultLibrary) {
            NSLog(@"[RendererBridge] Default shader library not found!");
            return false;
        }

        id<MTLFunction> vertexFunc = [defaultLibrary newFunctionWithName:@"vertex_main"];
        id<MTLFunction> fragmentFunc = [defaultLibrary newFunctionWithName:@"fragment_main"];

        // Render Pipeline State 생성
        MTLRenderPipelineDescriptor *pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDesc.vertexFunction = vertexFunc;
        pipelineDesc.fragmentFunction = fragmentFunc;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float; // 뎁스 포맷

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

    // 카메라의 View-Projection 행렬을 버퍼 인덱스 1번으로 바인딩
    [encoder setVertexBytes:&viewProjMatrix length:sizeof(simd_float4x4) atIndex:1];

    for (const auto& part : mesh->parts) {
        id<MTLBuffer> vertexBuffer = (__bridge id<MTLBuffer>)part.vertexBuffer;
        [encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];

        for (const auto& subMesh : part.subMeshes) {
            id<MTLBuffer> indexBuffer = (__bridge id<MTLBuffer>)subMesh.indexBuffer;
            MTLIndexType indexType = (subMesh.indexTypeBytes == 4) ? MTLIndexTypeUInt32 : MTLIndexTypeUInt16;

            [encoder drawIndexedPrimitives:(MTLPrimitiveType)subMesh.primitiveType
                                indexCount:subMesh.indexCount
                                 indexType:indexType
                               indexBuffer:indexBuffer
                         indexBufferOffset:0];
        } // for (const auto& subMesh : part.subMeshes)
    } // for (const auto& part : mesh->parts)
} // RendererBridge_DrawMesh
