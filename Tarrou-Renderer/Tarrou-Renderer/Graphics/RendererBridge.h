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

bool RendererBridge_InitPipeline(void*, const char*, const char*, void**, void**);
void RendererBridge_DrawMesh(void*, void*, void*, const void*, simd_float4x4);

#ifdef __cplusplus
}
#endif

#endif /* RendererBridge_h */
