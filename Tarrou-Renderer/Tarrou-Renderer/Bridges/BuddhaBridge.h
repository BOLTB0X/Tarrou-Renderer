//
//  BuddhaBridge.h
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#ifndef BuddhaBridge_h
#define BuddhaBridge_h

#include <stdbool.h>
#include <simd/simd.h>

#ifdef __cplusplus
extern "C" {
#endif

bool BuddhaBridge_InitMeshPipeline(void*       devicePtr,
                                   const char* shaderSourceRaw,
                                   const char* meshFunctionName,
                                   const char* fragmentFunctionName,
                                   void**      outPipelineState);

void BuddhaBridge_DrawMeshlets(void*  encoderPtr,
                               void*  pipelineStatePtr,
                               void*  depthStatePtr,
                               void*  meshletBuffer,
                               void*  meshletVerticesBuffer,
                               void*  meshletTrianglesBuffer,
                               void*  vertexBuffer,
                               size_t meshletCount, simd_float4x4 modelMatrix);

bool BuddhaBridge_InitMeshletsInctancePipeline(void*       devicePtr,
                                               const char* shaderSourceRaw,
                                               const char* objectFunctionName,
                                               const char* meshFunctionName,
                                               const char* fragmentFunctionName,
                                               size_t      payloadSize,
                                               void**      outPipelineState);

void BuddhaBridge_DrawMeshletsInctance(void* encoderPtr,
                                       void* pipelineStatePtr,
                                       void* depthStatePtr,
                                       void* meshletBuffer,
                                       void* meshletVerticesBuffer,
                                       void* meshletTrianglesBuffer,
                                       void* vertexBuffer,
                                       size_t vertexBufferOffset,
                                       void* instanceBuffer,
                                       size_t meshletCount,
                                       uint32_t instanceCount);

#ifdef __cplusplus
}
#endif

#endif /* BuddhaBridge_h */
