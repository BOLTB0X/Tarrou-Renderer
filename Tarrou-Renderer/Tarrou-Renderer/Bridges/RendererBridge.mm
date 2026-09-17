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

extern "C" bool RendererBridge_InitPipeline(void*       devicePtr,
                                            const char* vertexFunctionName,
                                            const char* fragmentFunctionName,
                                            void**      outPipelineState) {
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

extern "C" bool RendererBridge_InitMeshPipeline(void*       devicePtr,
                                                const char* meshFunctionName,
                                                const char* fragmentFunctionName,
                                                void**      outPipelineState) {
    @autoreleasepool {
        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;

        if (!device || !meshFunctionName || !fragmentFunctionName || !outPipelineState) return false;

        id<MTLLibrary> defaultLibrary = [device newDefaultLibrary];
        if (!defaultLibrary) {
            NSLog(@"[RendererBridge] Default shader library not found!");
            return false;
        }

        NSString* meshName = [NSString stringWithUTF8String:meshFunctionName];
        NSString* fragmentName = [NSString stringWithUTF8String:fragmentFunctionName];

        if (!meshName || !fragmentName) {
            NSLog(@"[RendererBridge] Invalid shader function name.");
            return false;
        }

        // Find Mesh / Fragment Function
        id<MTLFunction> meshFunc = [defaultLibrary newFunctionWithName:meshName];
        id<MTLFunction> fragmentFunc = [defaultLibrary newFunctionWithName:fragmentName];

        if (!meshFunc || !fragmentFunc) {
            NSLog(@"[RendererBridge] 함수를 찾지 못함: " @"mesh=%@ fragment=%@", meshName, fragmentName);
            return false;
        }

        // 4. Mesh Render Pipeline Descriptor
        MTLMeshRenderPipelineDescriptor* pipelineDesc = [[MTLMeshRenderPipelineDescriptor alloc] init];

        pipelineDesc.meshFunction = meshFunc;
        pipelineDesc.fragmentFunction = fragmentFunc;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        
        NSError* error = nil;
        id<MTLRenderPipelineState> pipelineState =
            [device newRenderPipelineStateWithMeshDescriptor:pipelineDesc
                                                      options:0
                                                   reflection:nil
                                                        error:&error];

        if (!pipelineState) {
            NSLog(@"[RendererBridge] Mesh Pipeline creation error: %@", error);
            return false;
        }

        *outPipelineState = (void*)CFBridgingRetain(pipelineState);

        return true;
    }
} // RendererBridge_InitMeshPipeline

extern "C" void RendererBridge_DrawMeshlets(void* encoderPtr,
                                            void* pipelineStatePtr,
                                            void* depthStatePtr,
                                            void* meshletBuffer,
                                            void* meshletVerticesBuffer,
                                            void* meshletTrianglesBuffer,
                                            void* vertexBuffer,
                                            size_t meshletCount,
                                            simd_float4x4 viewProjMatrix) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)encoderPtr;
    id<MTLRenderPipelineState> pipelineState = (__bridge id<MTLRenderPipelineState>)pipelineStatePtr;
    id<MTLDepthStencilState> depthState = (__bridge id<MTLDepthStencilState>)depthStatePtr;

    if (!encoder || !pipelineState) return;

    [encoder setRenderPipelineState:pipelineState];
    if (depthState) {
        [encoder setDepthStencilState:depthState];
    }

    // 버퍼 바인딩 (setMeshBuffer 사용)
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletBuffer offset:0 atIndex:0];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletVerticesBuffer offset:0 atIndex:1];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)meshletTrianglesBuffer offset:0 atIndex:2];
    [encoder setMeshBuffer:(__bridge id<MTLBuffer>)vertexBuffer offset:0 atIndex:3];
    
    // MVP 행렬 넘기기
    [encoder setMeshBytes:&viewProjMatrix length:sizeof(simd_float4x4) atIndex:4];

    // 드로우 콜 수행 (Grid당 메쉬렛 1개, Threadgroup당 스레드 128개)
    // 스레드 128개인 이유: 메쉬렛당 최대 정점(64)과 최대 삼각형(124)을 모두 커버할 수 있는 최소 단위
    MTLSize threadgroupsPerGrid = MTLSizeMake(meshletCount, 1, 1);
    MTLSize threadsPerThreadgroup = MTLSizeMake(128, 1, 1);

    [encoder drawMeshThreadgroups:threadgroupsPerGrid
      threadsPerObjectThreadgroup:MTLSizeMake(1, 1, 1)
        threadsPerMeshThreadgroup:threadsPerThreadgroup];
} // RendererBridge_DrawMeshlets

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
