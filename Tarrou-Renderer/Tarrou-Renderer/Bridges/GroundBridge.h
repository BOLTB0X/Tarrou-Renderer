//
//  GroundBridge.h
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#ifndef GroundBridge_h
#define GroundBridge_h
 
#include <stdbool.h>
#include <simd/simd.h>
 
#ifdef __cplusplus
extern "C" {
#endif
 
bool GroundBridge_InitPipeline(void*       devicePtr,
                               const char* shaderSourceRaw,
                               const char* vertexFunctionName,
                               const char* fragmentFunctionName,
                               void**      outPipelineState,
                               void**      outSamplerState);
 
void* GroundBridge_CreateVertexBuffer(void*,const void*,unsigned long);
 
void GroundBridge_Draw(void*         encoderPtr,
                       void*         pipelineStatePtr,
                       void*         depthStatePtr,
                       void*         vertexBufferPtr,
                       unsigned int  vertexCount,
                       void*         texturePtr,
                       void*         shadowTexturePtr,
                       void*         samplerStatePtr);
 
#ifdef __cplusplus
}
#endif
 
#endif /* GroundBridge_h */
