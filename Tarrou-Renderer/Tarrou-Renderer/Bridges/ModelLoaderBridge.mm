//
//  ModelLoaderBridge.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#import <Foundation/Foundation.h>
#import <ModelIO/ModelIO.h>
#import <MetalKit/MetalKit.h>

#include "MeshData.hpp"
#include "ModelLoader.hpp"
#include "ModelLoaderBridge.h"
#include "MetalResource.h"

extern "C" bool ModelLoaderBridge_LoadOBJ(const char* utf8Path, void* devicePtr, void* meshOutPtr) {
    @autoreleasepool {
        if (!utf8Path || !devicePtr || !meshOutPtr) { return false; }
        Mesh& meshOut = *reinterpret_cast<Mesh*>(meshOutPtr);

        id<MTLDevice> device = (__bridge id<MTLDevice>)devicePtr;
        if (!device) { return false; }

        NSString* nsPath = [NSString stringWithUTF8String:utf8Path];
        if (![[NSFileManager defaultManager] fileExistsAtPath:nsPath]) {
            NSLog(@"[ModelLoader] File not found: %@", nsPath);
            return false;
        }

        // allocator 및 MDLVertexDescriptor 생성
        MTKMeshBufferAllocator* allocator = [[MTKMeshBufferAllocator alloc] initWithDevice:device];

        MDLVertexDescriptor* vertexDescriptor = [[MDLVertexDescriptor alloc] init];

        vertexDescriptor.attributes[0] =
            [[MDLVertexAttribute alloc] initWithName:MDLVertexAttributePosition
                                              format:MDLVertexFormatFloat3
                                              offset:0
                                         bufferIndex:0];

        vertexDescriptor.attributes[1] =
            [[MDLVertexAttribute alloc] initWithName:MDLVertexAttributeNormal
                                              format:MDLVertexFormatFloat3
                                              offset:sizeof(float) * 3
                                         bufferIndex:0];

        vertexDescriptor.layouts[0] =
            [[MDLVertexBufferLayout alloc] initWithStride:sizeof(float) * 6];

        NSURL* url = [NSURL fileURLWithPath:nsPath];

        MDLAsset* asset = [[MDLAsset alloc] initWithURL:url
                                       vertexDescriptor:vertexDescriptor
                                        bufferAllocator:allocator];
        if (!asset) { return false; }

        [asset loadTextures];

        NSError* error = nil;
        NSMutableArray<MTKMesh*>* mtkMeshes = [NSMutableArray array];

        for (MDLObject* obj in asset) {
            if (![obj isKindOfClass:[MDLMesh class]]) { continue; }
            MDLMesh* mdlMesh = (MDLMesh*)obj;
            MTKMesh* mtkMesh = [[MTKMesh alloc] initWithMesh:mdlMesh device:device error:&error];
            if (!mtkMesh || error) {
                NSLog(@"[ModelLoader] MTKMesh init error: %@", error);
                return false;
            }
            [mtkMeshes addObject:mtkMesh];
        } // for (MDLObject* obj in asset)

        // 새로 로드하기 전에 기존 GPU 버퍼 해제
        meshOut.parts.clear();
        meshOut.parts.reserve(mtkMeshes.count);

        for (MTKMesh* mtkMesh in mtkMeshes) {
            if (mtkMesh.vertexBuffers.count == 0) { continue; }
            id<MTLBuffer> vb = mtkMesh.vertexBuffers[0].buffer;
            if (!vb) { continue; }

            // MeshPart 오프셋 및 데이터 갱신
            MeshPart part;
            part.vertexBuffer = MetalResource::Adopt((void*)CFBridgingRetain(vb));
            part.vertexBufferOffset = mtkMesh.vertexBuffers[0].offset;

            NSUInteger stride = 0;
            if (mtkMesh.vertexDescriptor.layouts.count > 0) {
                stride = mtkMesh.vertexDescriptor.layouts[0].stride;
            }
            part.vertexStride = static_cast<uint32_t>(stride);

            if (vb.length < part.vertexBufferOffset) {
                return false;
            }

            part.vertexCount = static_cast<uint32_t>(mtkMesh.vertexCount);

            for (MTKSubmesh* sm in mtkMesh.submeshes) {
                MeshSubmesh s;
                id<MTLBuffer> ib = sm.indexBuffer.buffer;
                // MeshSubmesh 오프셋 및 데이터 갱신
                s.indexBuffer = MetalResource::Adopt((void*)CFBridgingRetain(ib));
                s.indexBufferOffset = sm.indexBuffer.offset;
                s.indexCount = static_cast<uint32_t>(sm.indexCount);
                s.indexTypeBytes = (sm.indexType == MTLIndexTypeUInt32) ? 4 : 2;
                s.primitiveType = static_cast<uint32_t>(sm.primitiveType);

                part.subMeshes.push_back(std::move(s)); // MetalResource는 move-only
            }

            meshOut.parts.push_back(std::move(part)); // MetalResource는 move-only
        } // for (MTKMesh* mtkMesh in mtkMeshes)

        return !meshOut.parts.empty();
    } // @autoreleasepool
} // ModelLoaderBridge_LoadOBJ
