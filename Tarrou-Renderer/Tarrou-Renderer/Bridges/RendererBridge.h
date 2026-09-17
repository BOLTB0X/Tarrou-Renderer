//
//  RendererBridge.h
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#ifndef RendererBridge_h
#define RendererBridge_h

#include <stdbool.h>
#include <simd/simd.h>

#ifdef __cplusplus
extern "C" {
#endif

void* RendererBridge_CreateDepthState(void*);
bool  RendererBridge_InitPipeline(void*, const char*, const char*, void**);
bool  RendererBridge_InitMeshPipeline(void*, const char*, const char*, void**);
void  RendererBridge_DrawMesh(void*, void*, void*, const void*, simd_float4x4);
void  RendererBridge_DrawMeshlets(void*, void*, void*, void*,void*, void*, void*, size_t, simd_float4x4);
void* RendererBridge_GetBufferContents(void*);
void* RendererBridge_CreateBuffer(void*, const void*, size_t);

#ifdef __cplusplus
}
#endif

#endif /* RendererBridge_h */
