//
//  ShadowMapBridge.h
//  Tarrou-Renderer
//
//  Created by B0X on 9/20/26.
//

#ifndef ShadowMapBridge_h
#define ShadowMapBridge_h

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
void* ShadowMapBridge_CreateShadowDepthTexture(void* devicePtr, int width, int height);
void* ShadowMapBridge_CreateShadowPassDescriptor(void* depthTexturePtr);
void  ShadowMapBridge_ReleaseMetalObject(void* metalObject);
bool  ShadowMapBridge_InitShadowPipeline(void* devicePtr,
                                         const char* shaderSourceRaw,
                                         const char* objectFunctionName,
                                         const char* meshFunctionName,
                                         size_t      payloadSize,
                                         void**      outPipelineState);

#ifdef __cplusplus
}
#endif

#endif /* ShadowMapBridge_h */
