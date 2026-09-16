//
//  Ground.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#include "Ground.hpp"
#include "GlobalVariables.hpp"
#include "GroundBridge.h"
#include "TextureLoader.hpp"
#include "Texture.hpp"

using namespace GlobalVariables;

Ground::Ground() {
} // Ground

Ground::~Ground() {
} // ~Ground

bool Ground::Init(void* device) {
    TextureLoader loader(device);
    m_texture = loader.Load(GetAssetPath(GROUND_TEXTURE_RELATIVE_PATH));
    if (!m_texture) {
        return false;
    }

    const float half   = GROUND_SIZE * 0.5f;
    const float tiling = GROUND_TEXTURE_TILING;

    m_vertices = {
        { { -half, GROUND_HEIGHT, -half }, { 0.0f,   0.0f   } },
        { {  half, GROUND_HEIGHT, -half }, { tiling, 0.0f   } },
        { {  half, GROUND_HEIGHT,  half }, { tiling, tiling } },

        { { -half, GROUND_HEIGHT, -half }, { 0.0f,   0.0f   } },
        { {  half, GROUND_HEIGHT,  half }, { tiling, tiling } },
        { { -half, GROUND_HEIGHT,  half }, { 0.0f,   tiling } },
    };

    void* rawPipelineState = nullptr;
    void* rawSamplerState  = nullptr;
    if (!GroundBridge_InitPipeline(device,
                                    GROUND_SHADER_VERTEX,
                                    GROUND_SHADER_FRAGMENT,
                                    &rawPipelineState,
                                    &rawSamplerState)) {
        return false;
    }
    m_pipelineState = MetalResource::Adopt(rawPipelineState);
    m_samplerState  = MetalResource::Adopt(rawSamplerState);

    void* rawVertexBuffer = GroundBridge_CreateVertexBuffer(
        device,
        m_vertices.data(),
        m_vertices.size() * sizeof(Vertex)
    );
    if (!rawVertexBuffer) {
        return false;
    }
    m_vertexBuffer = MetalResource::Adopt(rawVertexBuffer);

    return true;
} // Init

void Ground::Render(void* renderCommandEncoder, void* depthState, simd_float4x4 viewProjMatrix) {
    if (!m_pipelineState || !m_vertexBuffer || !m_texture) return;

    GroundBridge_Draw(
        renderCommandEncoder,
        m_pipelineState.Get(),
        depthState,
        m_vertexBuffer.Get(),
        static_cast<unsigned int>(m_vertices.size()),
        m_texture->GetNativeTexture(),
        m_samplerState.Get(),
        viewProjMatrix
    );
} // Render

float Ground::GetHeight() const {
    return GROUND_HEIGHT;
} // GetHeight
