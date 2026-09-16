//
//  Renderer.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#include "Renderer.hpp"
#include "Camera.hpp"
#include "GlobalVariables.hpp"
#include "RendererBridge.h"
#include "Buddha.hpp"
#include "Ground.hpp"

Renderer::Renderer() {
    m_clearColor = ClearColor();
    m_device = nullptr;
    m_Camera = std::make_unique<Camera>();
    m_Buddha = std::make_unique<Buddha>();
    m_Ground = std::make_unique<Ground>();
} // Renderer

Renderer::~Renderer() {
    m_device = nullptr;
} // Renderer

bool Renderer::Init(void* metalDevice, float width, float height) {
    m_device = metalDevice;
    if (!m_device) return false;

    Camera::InitParam camParam;
    camParam.fov = GlobalVariables::DEFAULT_FOV;
    camParam.aspect = (height > 0.0f) ? (width / height) : (16.0f / 9.0f);
    camParam.screenNear = GlobalVariables::SCREEN_NEAR;
    camParam.screenFar = GlobalVariables::SCREEN_DEPTH;
    m_Camera->Init(camParam);

    m_depthState = MetalResource::Adopt(RendererBridge_CreateDepthState(m_device));
    if (!m_depthState) return false;

    if (!m_Buddha->Init(m_device)) {
        return false;
    }

    if (!m_Ground->Init(m_device)) {
        return false;
    } else m_Buddha->SetPosition({ 0.0f, m_Ground->GetHeight() + GlobalVariables::BUDDHA_OFFSET, 0.0f });

    return true;
} // Init

void Renderer::Update(const UpdateParam& param) {
    Camera::FrameParam camParam;
    camParam.moveForward = param.moveForward;
    camParam.moveRight = param.moveRight;
    camParam.moveUp = param.moveUp;
    camParam.rotationDeltaX = param.rotationDeltaX;
    camParam.rotationDeltaY = param.rotationDeltaY;
    camParam.zoomDelta = param.zoomDelta;

    m_Camera->Frame(camParam);
} // Update

void Renderer::Render(void* renderCommandEncoder) {
    if (!renderCommandEncoder) return;
    simd_float4x4 viewMat = m_Camera->GetViewMatrix();
    simd_float4x4 projMat = m_Camera->GetReverseZProjectionMatrix();

    simd_float4x4 viewProjMatrix = simd_mul(projMat, viewMat);

    m_Ground->Render(renderCommandEncoder, m_depthState.Get(), viewProjMatrix);
    m_Buddha->Render(renderCommandEncoder, m_depthState.Get(), viewProjMatrix);
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
