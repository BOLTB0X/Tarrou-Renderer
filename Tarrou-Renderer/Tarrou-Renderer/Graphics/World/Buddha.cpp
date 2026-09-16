//
//  Buddha.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#include "Buddha.hpp"
#include "GlobalVariables.hpp"
#include "ModelLoader.hpp"
#include "RendererBridge.h"

using namespace GlobalVariables;

namespace {
    simd_float4x4 MakeTranslationMatrix(simd_float3 t) {
        simd_float4x4 m = matrix_identity_float4x4;
        m.columns[3] = simd_make_float4(t.x, t.y, t.z, 1.0f);
        return m;
    } // MakeTranslationMatrix
} // namespace

Buddha::Buddha() {
} // Buddha

Buddha::~Buddha() {
} // ~Buddha

bool Buddha::Init(void* device) {
    std::string modelPath = GetAssetPath(MODEL_RELATIVE_PATH);
    if (!ModelLoader::LoadOBJ(modelPath, device, m_mesh)) {
        return false;
    }

    void* rawPipelineState = nullptr;
    if (!RendererBridge_InitPipeline(device,
                                      DEFAULT_BUDDHA_SHADER_VERTEX,
                                      DEFAULT_BUDDHA_SHADER_FRAGMENT,
                                      &rawPipelineState)) {
        return false;
    }
    m_pipelineState = MetalResource::Adopt(rawPipelineState);

    return true;
} // Init

void Buddha::Render(void* renderCommandEncoder, void* depthState, simd_float4x4 viewProjMatrix) {
    if (!m_pipelineState) return;

    simd_float4x4 modelMatrix = MakeTranslationMatrix(m_position);
    simd_float4x4 mvp = simd_mul(viewProjMatrix, modelMatrix);

    RendererBridge_DrawMesh(renderCommandEncoder, m_pipelineState.Get(), depthState, &m_mesh, mvp);
} // Render

void Buddha::SetPosition(simd_float3 position) {
    m_position = position;
} // SetPosition

simd_float3 Buddha::GetPosition() const {
    return m_position;
} // GetPosition
