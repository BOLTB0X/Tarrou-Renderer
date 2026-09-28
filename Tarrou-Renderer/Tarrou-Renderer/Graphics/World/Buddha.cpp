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
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
 
using namespace GlobalVariables;

namespace {
struct ConeDebugVertex {
    float position[3];
};
}
 
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

    std::vector<ConeDebugVertex> coneVertices;
    constexpr uint32_t coneSegments = 12;
    constexpr float pi = 3.14159265358979323846f;
    for (const auto& meshletData : m_meshletsList) {
        for (const MeshletBounds& bounds : meshletData->m_meshletBounds) {
            if (bounds.coneCutoff <= -1.0f || bounds.coneCutoff >= 1.0f) continue;

            simd_float3 apex = simd_make_float3(bounds.coneApex[0], bounds.coneApex[1], bounds.coneApex[2]);
            simd_float3 axis = simd_normalize(simd_make_float3(bounds.coneAxis[0], bounds.coneAxis[1], bounds.coneAxis[2]));
            simd_float3 helper = std::abs(axis.y) < 0.9f
                ? simd_make_float3(0.0f, 1.0f, 0.0f)
                : simd_make_float3(1.0f, 0.0f, 0.0f);
            simd_float3 basisU = simd_normalize(simd_cross(axis, helper));
            simd_float3 basisV = simd_cross(axis, basisU);

            const float coneLength = std::max(bounds.radius * 0.35f, 0.02f);
            const float coneAngle = std::acos(std::clamp(bounds.coneCutoff, -0.99f, 0.99f));
            const float baseRadius = coneLength * std::tan(coneAngle);
            const simd_float3 baseCenter = apex + axis * coneLength;

            auto appendLine = [&coneVertices](simd_float3 start, simd_float3 end) {
                coneVertices.push_back({{start.x, start.y, start.z}});
                coneVertices.push_back({{end.x, end.y, end.z}});
            };

            for (uint32_t segment = 0; segment < coneSegments; ++segment) {
                const float angle0 = (2.0f * pi * segment) / coneSegments;
                const float angle1 = (2.0f * pi * (segment + 1)) / coneSegments;
                const simd_float3 radial0 = basisU * std::cos(angle0) + basisV * std::sin(angle0);
                const simd_float3 radial1 = basisU * std::cos(angle1) + basisV * std::sin(angle1);
                const simd_float3 ring0 = baseCenter + radial0 * baseRadius;
                const simd_float3 ring1 = baseCenter + radial1 * baseRadius;
                appendLine(apex, ring0);
                appendLine(ring0, ring1);
            }
        }
    }

    if (!coneVertices.empty()) {
        m_coneDebugVertexBuffer = MetalResource::Adopt(RendererBridge_CreateBuffer(
            device, coneVertices.data(), coneVertices.size() * sizeof(ConeDebugVertex)));
        if (m_coneDebugVertexBuffer) {
            m_coneDebugVertexCount = static_cast<uint32_t>(coneVertices.size());
        }
    }
 
    void* rawPipelineState = nullptr;
    std::string shaderFilePath = GetRootPath(MESH_SHADER);
    std::string baseDirPath = GetRootPath(BASE_PATH);
    std::string buddhaSource = ShaderLoader::Load(shaderFilePath, baseDirPath);
    const size_t objectPayloadSize = sizeof(uint32_t) * 3;
 
    if (!BuddhaBridge_InitMeshletsInctancePipeline(device, buddhaSource.c_str(),
                                                   MESH_SHADER_OBJECT, MESH_SHADER_MESH, MESH_SHADER_FRAGMENT,
                                                   objectPayloadSize,
                                                   &rawPipelineState)) {
        return false;
    }
    m_pipelineState = MetalResource::Adopt(rawPipelineState);

    void* rawConeDebugPipelineState = nullptr;
    void* rawConeDebugDepthState = nullptr;
    if (BuddhaBridge_InitConeDebugPipeline(device,
                                           buddhaSource.c_str(),
                                           "BuddhaConeDebugVS",
                                           "BuddhaConeDebugPS",
                                           &rawConeDebugPipelineState,
                                           &rawConeDebugDepthState)) {
        m_coneDebugPipelineState = MetalResource::Adopt(rawConeDebugPipelineState);
        m_coneDebugDepthState = MetalResource::Adopt(rawConeDebugDepthState);
    }
    void* rawShadowPipelineState = nullptr;
    std::string shadowShaderFilePath = GlobalVariables::GetRootPath(SHADOW_MESH_SHADER);
    std::string shadowSource = ShaderLoader::Load(shadowShaderFilePath, baseDirPath);
        
    if (!ShadowMapBridge_InitShadowPipeline(device,
                                            shadowSource.c_str(),
                                            SHADOW_SHADER_OBJECT,
                                            SHADOW_SHADER_MESH,
                                            sizeof(uint32_t) * 2,
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
                    bool enableNormalConeCulling,
                    bool debugConeCandidates,
                    bool showNormalConeGizmos,
                    uint32_t coneDebugStart,
                    uint32_t coneDebugCount) {
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
                                                           (enableNormalConeCulling ? 1.0f : 0.0f) +
                                                               (debugConeCandidates ? 2.0f : 0.0f)),
                                          meshletData->m_meshlets.size(),
                                          m_instanceCount);
        partIndex++;
    }

    constexpr uint32_t verticesPerCone = 48;
    const uint32_t totalConeCount = GetConeDebugCount();
    const uint32_t boundedStart = std::min(coneDebugStart, totalConeCount);
    const uint32_t boundedCount = std::min(coneDebugCount, totalConeCount - boundedStart);
    if (showNormalConeGizmos && m_coneDebugPipelineState && m_coneDebugVertexBuffer && boundedCount > 0) {
        BuddhaBridge_DrawConeDebug(renderCommandEncoder,
                                   m_coneDebugPipelineState.Get(),
                                   m_coneDebugDepthState.Get(),
                                   m_coneDebugVertexBuffer.Get(),
                                   boundedStart * verticesPerCone,
                                   boundedCount * verticesPerCone,
                                   m_instanceBuffer.Get(),
                                   m_instanceCount);
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

uint32_t Buddha::GetConeDebugCount() const {
    return m_coneDebugVertexCount / 48;
}
 
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
