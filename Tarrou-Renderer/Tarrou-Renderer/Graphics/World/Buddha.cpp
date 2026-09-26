//
//  Buddha.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#include "Buddha.hpp"
#include "MeshletBuilder.hpp"
#include "Meshlet.hpp"
#include "ModelLoader.hpp"
#include "ShaderLoader.hpp"
#include "GlobalVariables.hpp"
#include "MathHelper.hpp"
#include "DebugHelper.hpp"
//
#include "BuddhaBridge.h"
#include "ShadowMapBridge.h"
#include "RendererBridge.h"
// STL
#include <iostream>
#include <string>
 
using namespace GlobalVariables;
 
Buddha::Buddha() {
} // Buddha
 
Buddha::~Buddha() {
} // ~Buddha
 
bool Buddha::Init(void* device) {
    std::string modelPath = GetRootPath(MODEL_RELATIVE_PATH);
    if (!ModelLoader::LoadOBJ(modelPath, device, m_mesh)) {
        return false;
    }
 
    m_meshletsList = MeshletBuilder::Build(m_mesh, device);
    if (m_meshletsList.empty()) { return false; }
 
    void* rawPipelineState = nullptr;
    std::string shaderFilePath = GetRootPath(MESH_SHADER);
    std::string baseDirPath = GetRootPath(BASE_PATH);
    std::string buddhaSource = ShaderLoader::Load(shaderFilePath, baseDirPath);
    const size_t objectPayloadSize = sizeof(uint32_t) * 2;
 
    if (!BuddhaBridge_InitMeshletsInctancePipeline(device, buddhaSource.c_str(),
                                                   MESH_SHADER_OBJECT, MESH_SHADER_MESH, MESH_SHADER_FRAGMENT,
                                                   objectPayloadSize,
                                                   &rawPipelineState)) {
        return false;
    }
    m_pipelineState = MetalResource::Adopt(rawPipelineState);
    void* rawShadowPipelineState = nullptr;
    std::string shadowShaderFilePath = GlobalVariables::GetRootPath(SHADOW_MESH_SHADER);
    std::string shadowSource = ShaderLoader::Load(shadowShaderFilePath, baseDirPath);
        
    if (!ShadowMapBridge_InitShadowPipeline(device,
                                            shadowSource.c_str(),
                                            SHADOW_SHADER_OBJECT,
                                            SHADOW_SHADER_MESH,
                                            objectPayloadSize,
                                            &rawShadowPipelineState)) { return false; }
    m_shadowPipelineState = MetalResource::Adopt(rawShadowPipelineState);
    
    uint32_t maxVertexIndex = 0;
    for (uint32_t idx : m_meshletsList[0]->m_meshletVertices) {
        maxVertexIndex = std::max(maxVertexIndex, idx);
    }
    return true;
} // Init
 
void Buddha::Render(void* renderCommandEncoder,
                    void* depthState,
                    const simd_float4* frustumPlanes,
                    simd_float3 cameraPosition,
                    bool enableNormalConeCulling) {
    if (!m_pipelineState || m_instanceCount == 0) return;
 
    int partIndex = 0;
    for (const auto& meshletData : m_meshletsList) {
        BuddhaBridge_DrawMeshletsInctance(renderCommandEncoder,
                                          m_pipelineState.Get(),
                                          depthState,
                                          meshletData->m_meshletBuffer.Get(),
                                          meshletData->m_meshletVerticesBuffer.Get(),
                                          meshletData->m_meshletTrianglesBuffer.Get(),
                                          meshletData->m_meshletBoundsBuffer.Get(),
                                          m_mesh.parts[partIndex].vertexBuffer.Get(),
                                          m_mesh.parts[partIndex].vertexBufferOffset,
                                          m_instanceBuffer.Get(),
                                          frustumPlanes,
                                          simd_make_float4(cameraPosition.x,
                                                           cameraPosition.y,
                                                           cameraPosition.z,
                                                           enableNormalConeCulling ? 1.0f : 0.0f),
                                          meshletData->m_meshlets.size(),
                                          m_instanceCount);
        partIndex++;
    }
} // Render

void Buddha::RenderShadow(void* renderCommandEncoder, void* depthState) {
    if (!m_shadowPipelineState || m_instanceCount == 0) return;

    int partIndex = 0;
    for (const auto& meshletData : m_meshletsList) {
        BuddhaBridge_DrawMeshletsInctance(renderCommandEncoder,
                                          m_shadowPipelineState.Get(),
                                          depthState,
                                          meshletData->m_meshletBuffer.Get(),
                                          meshletData->m_meshletVerticesBuffer.Get(),
                                          meshletData->m_meshletTrianglesBuffer.Get(),
                                          meshletData->m_meshletBoundsBuffer.Get(),
                                          m_mesh.parts[partIndex].vertexBuffer.Get(),
                                          m_mesh.parts[partIndex].vertexBufferOffset,
                                          m_instanceBuffer.Get(),
                                          nullptr,
                                          simd_make_float4(0.0f, 0.0f, 0.0f, 0.0f),
                                          meshletData->m_meshlets.size(),
                                          m_instanceCount);
        partIndex++;
    }
} // RenderShadow
 
void Buddha::SetInstances(void* device, const std::vector<simd_float3>& positions) {
    std::cout << "[Buddha] SetInstances called, positions.size()=" << positions.size() << std::endl;
    std::vector<simd_float4x4> transforms;
    transforms.reserve(positions.size());
    for (const auto& p : positions) {
        transforms.push_back(MathHelper::MakeTranslationMatrix(p));
    }
 
    void* rawBuffer = RendererBridge_CreateBuffer(
        device,
        transforms.data(),
        transforms.size() * sizeof(simd_float4x4)
    );
    if (!rawBuffer) { return; }
 
    m_instanceBuffer = MetalResource::Adopt(rawBuffer);
    m_instanceCount = static_cast<uint32_t>(transforms.size());
} // SetInstances
