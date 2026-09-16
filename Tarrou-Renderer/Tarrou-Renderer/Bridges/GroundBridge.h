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
 
// 파이프라인 + 샘플러 상태 생성 (한 번만 호출)
bool GroundBridge_InitPipeline(void*       devicePtr,
                               const char* vertexFunctionName,
                               const char* fragmentFunctionName,
                               void**      outPipelineState,
                               void**      outSamplerState);
 
void* GroundBridge_CreateVertexBuffer(void*         devicePtr,
                                      const void*   vertexData,
                                      unsigned long byteLength);
 
void GroundBridge_Draw(void*         encoderPtr,
                       void*         pipelineStatePtr,
                       void*         depthStatePtr,
                       void*         vertexBufferPtr,
                       unsigned int  vertexCount,
                       void*         texturePtr,
                       void*         samplerStatePtr,
                       simd_float4x4 viewProjMatrix);
 
#ifdef __cplusplus
}
#endif
 
#endif /* GroundBridge_h */
