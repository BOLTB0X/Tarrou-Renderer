//
//  Renderer.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#include "Renderer.hpp"
#include "Camera.hpp"
#include "GlobalVariables.hpp"
#include "ModelLoader.hpp"
#include "RendererBridge.h"

using namespace GlobalVariables;

Renderer::Renderer() {
    m_clearColor = ClearColor();
    m_device = nullptr;
    m_Camera = std::make_unique<Camera>();
} // Renderer

Renderer::~Renderer() {
    m_device = nullptr;
} // Renderer

bool Renderer::Init(void* metalDevice, float width, float height) {
    m_device = metalDevice;
    if (!m_device) return false;
    
    Camera::InitParam camParam;
    camParam.fov = DEFAULT_FOV;
    camParam.aspect = (height > 0.0f) ? (width / height) : (16.0f / 9.0f);;
    camParam.screenNear = SCREEN_NEAR;
    camParam.screenFar = SCREEN_DEPTH;
    m_Camera->Init(camParam);

    // 1. 모델 로드
    std::string modelPath = "/Users/b0x/Documents/GitHub/Tarrou-Renderer/Tarrou-Renderer/Assets/buddha.obj";
    if (!ModelLoader::LoadOBJ(modelPath, m_device, m_mesh)) {
        return false;
    }

        // 2. 파이프라인 및 뎁스 상태 초기화
    if (!RendererBridge_InitPipeline(m_device, &m_pipelineState, &m_depthState)) {
        return false;
    }
    return true;
} // Init

void Renderer::Update(float deltaTime) {

} // Update

void Renderer::Render(void* renderCommandEncoder) {
    if (!renderCommandEncoder) return;
    simd_float4x4 viewMat = m_Camera->GetViewMatrix();
    simd_float4x4 projMat = m_Camera->GetReverseZProjectionMatrix();
 
    simd_float4x4 viewProjMatrix = simd_mul(projMat, viewMat);
    RendererBridge_DrawMesh(renderCommandEncoder, m_pipelineState, m_depthState, &m_mesh, viewProjMatrix);
} // Render

void Renderer::OnResize(float width, float height) {
    if (height > 0.0f) {
        m_Camera->SetAspect(width / height);
    }
} // OnResize

void Renderer::OnGUI() {
    m_Camera->OnGUI();
} // OnGUI

Renderer::ClearColor& Renderer::GetClearColor() { return m_clearColor; }
void                  Renderer::SetClearColor(float r, float g, float b, float a) { m_clearColor = ClearColor(r, g, b, a); }
