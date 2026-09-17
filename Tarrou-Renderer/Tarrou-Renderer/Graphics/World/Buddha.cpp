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
#include "GlobalVariables.hpp"
#include "MathHelper.hpp"
#include "DebugHelper.hpp"
#include "RendererBridge.h"
// STL
#include <iostream>

using namespace GlobalVariables;

Buddha::Buddha() {
} // Buddha

Buddha::~Buddha() {
} // ~Buddha

bool Buddha::Init(void* device) {
    std::string modelPath = GetAssetPath(MODEL_RELATIVE_PATH);
    if (!ModelLoader::LoadOBJ(modelPath, device, m_mesh)) {
        return false;
    }
    
    m_meshletData = MeshletBuilder::Build(m_mesh, device);
    if (!m_meshletData) { return false; }
    
    void* rawPipelineState = nullptr;
//    if (!RendererBridge_InitPipeline(device,
//                                      DEFAULT_BUDDHA_SHADER_VERTEX,
//                                      DEFAULT_BUDDHA_SHADER_FRAGMENT,
//                                      &rawPipelineState)) {
//        return false;
//    }
    if (!RendererBridge_InitMeshPipeline(device, MESH_SHADER_MESH, MESH_SHADER_FRAGMENT, &rawPipelineState)) {
            return false;
    }
    m_pipelineState = MetalResource::Adopt(rawPipelineState);

    return true;
} // Init

void Buddha::Render(void* renderCommandEncoder, void* depthState, simd_float4x4 viewProjMatrix) {
    if (!m_pipelineState) return;

    simd_float4x4 modelMatrix = MathHelper::MakeTranslationMatrix(m_position);
    simd_float4x4 mvp = simd_mul(viewProjMatrix, modelMatrix);

//    RendererBridge_DrawMesh(renderCommandEncoder, m_pipelineState.Get(), depthState, &m_mesh, mvp);
    // Meshlet을 그리는 전용 함수 호출
    RendererBridge_DrawMeshlets(renderCommandEncoder,
                                m_pipelineState.Get(),
                                depthState,
                                m_meshletData->m_meshletBuffer.Get(),
                                m_meshletData->m_meshletVerticesBuffer.Get(),
            m_meshletData->m_meshletTrianglesBuffer.Get(),
            m_mesh.parts[0].vertexBuffer.Get(),       // 원본 Vertex Buffer
            m_meshletData->m_meshlets.size(),         // 총 메쉬렛 개수
            mvp
        );
} // Render


void        Buddha::SetPosition(simd_float3 position) { m_position = position; }
simd_float3 Buddha::GetPosition() const { return m_position; }

