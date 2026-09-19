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
void* RendererBridge_GetBufferContents(void*);
void* RendererBridge_CreateBuffer(void*, const void*, size_t);
void  RendererBridge_BindConstantBuffers(void*, const void*, size_t, const void*, size_t);

#ifdef __cplusplus
}
#endif

#endif /* RendererBridge_h */
