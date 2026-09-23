//
//  Renderer.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#include "Renderer.hpp"
#include "Camera.hpp"
#include "DirectionalLight.hpp"
#include "GlobalVariables.hpp"
#include "CommonConstantBuffer.hpp"
#include "RendererBridge.h"
#include "ShadowMapBridge.h"
#include "Buddha.hpp"
#include "Ground.hpp"
#include "MathHelper.hpp"
#include "imgui.h"

Renderer::Renderer() {
    m_clearColor = ClearColor();
    m_device = nullptr;
    m_Camera = std::make_unique<Camera>();
    m_DirLight = std::make_unique<DirectionalLight>();
    m_Buddha = std::make_unique<Buddha>();
    m_Ground = std::make_unique<Ground>();
    m_CommonCB = std::make_unique<CommonConstantBuffer>();
} // Renderer

Renderer::~Renderer() {
    m_device = nullptr;
} // Renderer

bool Renderer::Init(void* metalDevice, float width, float height) {
    m_device = metalDevice;
    if (!m_device) return false;
    
    m_screenWidth = width;
    m_screenHeight = height;

    Camera::InitParam camParam;
    camParam.fov = GlobalVariables::DEFAULT_FOV;
    camParam.aspect = (height > 0.0f) ? (width / height) : (16.0f / 9.0f);
    camParam.screenNear = GlobalVariables::SCREEN_NEAR;
    camParam.screenFar = GlobalVariables::SCREEN_DEPTH;
    
    m_Camera->Init(camParam);
    m_DirLight->Init();

    m_depthState = MetalResource::Adopt(RendererBridge_CreateDepthState(m_device));
    if (!m_depthState) return false;
    
    m_shadowTexture = MetalResource::Adopt(ShadowMapBridge_CreateShadowDepthTexture(m_device, GlobalVariables::SHADOW_MAP_WIDTH, GlobalVariables::SHADOW_MAP_HEIGHT));
    m_shadowPassDescriptor = MetalResource::Adopt(ShadowMapBridge_CreateShadowPassDescriptor(m_shadowTexture.Get()));
    m_shadowSampler = MetalResource::Adopt(RendererBridge_CreateShadowSampler(m_device));

    if (!m_Buddha->Init(m_device)) { return false; }

    if (!m_Ground->Init(m_device)) {
        return false;
    } else {
        std::vector<simd_float3> buddhaInstances = {
            simd_make_float3(0.0f, m_Ground->GetHeight() + GlobalVariables::BUDDHA_OFFSET, 0.0f),
            simd_make_float3(-3.0f, m_Ground->GetHeight() + GlobalVariables::BUDDHA_OFFSET, 0.0f),
            simd_make_float3(3.0f, m_Ground->GetHeight() + GlobalVariables::BUDDHA_OFFSET, 3.0f),
            simd_make_float3(-1.0f, m_Ground->GetHeight() + GlobalVariables::BUDDHA_OFFSET, -1.0f),
            simd_make_float3(-2.5f, m_Ground->GetHeight() + GlobalVariables::BUDDHA_OFFSET, -1.7f)
        };
        m_Buddha->SetInstances(m_device, buddhaInstances);
    }

    return true;
} // Init

void Renderer::Update(const UpdateParam& param) {
    m_globalTime += param.deltaTime;
    m_DirLight->Frame();
    
    Camera::FrameParam camParam;
    camParam.moveForward = param.moveForward;
    camParam.moveRight = param.moveRight;
    camParam.moveUp = param.moveUp;
    camParam.rotationDeltaX = param.rotationDeltaX;
    camParam.rotationDeltaY = param.rotationDeltaY;
    camParam.zoomDelta = param.zoomDelta;

    m_Camera->Frame(camParam);
    
    CommonConstantBuffer::FrameCB frameCB;
    frameCB.view = m_Camera->GetViewMatrix();
    frameCB.projection = m_Camera->GetReverseZProjectionMatrix();
    frameCB.viewInv = simd_inverse(frameCB.view);
    frameCB.projInv = simd_inverse(frameCB.projection);
    frameCB.cameraPosition = m_Camera->GetPosition();
    frameCB.cameraFov = m_Camera->GetFov();
    frameCB.screenResolution = simd_make_float2(m_screenWidth, m_screenHeight);
    frameCB.time = m_globalTime;
    frameCB.fPadding = 0.0f;
    m_CommonCB->UpdateFrameCB(frameCB);

    CommonConstantBuffer::DirectionalLightCB lightCB;
    lightCB.lightDir = m_DirLight->GetDirection();
    lightCB.lightAmbient = m_DirLight->GetAmbient();
    lightCB.lightDiffuse = m_DirLight->GetDiffuse();
    lightCB.lightLookAt = m_DirLight->GetLookAt();
    lightCB.lightViewMatrix = m_DirLight->GetViewMatrix();
    lightCB.lightProjectionMatrix = m_DirLight->GetProjectionMatrix();
    lightCB.shadowMapWidth = GlobalVariables::SHADOW_MAP_WIDTH;
    lightCB.shadowMapHeight = GlobalVariables::SHADOW_MAP_HEIGHT;
    lightCB.shadowBias = GlobalVariables::SHADOW_BIAS;
    lightCB.shadowSpread = GlobalVariables::SHADOW_SPREAD;

    m_CommonCB->UpdateLightCB(lightCB);
} // Update

void Renderer::Render(void* commandBuffer, void* mainPassDescriptor) {
    if (!commandBuffer || !mainPassDescriptor) return;
    
    void* shadowEncoder = RendererBridge_BeginRenderPass(commandBuffer, m_shadowPassDescriptor.Get());
    if (shadowEncoder) {
        m_CommonCB->Bind(shadowEncoder, nullptr, nullptr); // 빛 행렬(LightCB) 바인딩
        m_Buddha->RenderShadow(shadowEncoder, m_depthState.Get());
        RendererBridge_EndEncoding(shadowEncoder);
    }

    // ==========================================
    // Pass 2: Main Render (카메라 시점)
    // ==========================================
    void* mainEncoder = RendererBridge_BeginRenderPass(commandBuffer, mainPassDescriptor);
    if (mainEncoder) {
        m_CommonCB->Bind(mainEncoder, m_shadowTexture.Get(), m_shadowSampler.Get());
        m_Ground->Render(mainEncoder, m_depthState.Get());
        m_Buddha->Render(mainEncoder, m_depthState.Get());
        RendererBridge_EndEncoding(mainEncoder);
    }
} // Render

void Renderer::OnResize(float width, float height) {
    if (height > 0.0f) {
        m_Camera->SetAspect(width / height);
    }
    
    m_screenWidth = width;
    m_screenHeight = height;
} // OnResize

void Renderer::OnGUI() {
    m_Camera->OnGUI();
    m_DirLight->OnGUI();
} // OnGUI

Renderer::ClearColor& Renderer::GetClearColor() { return m_clearColor; }
void                  Renderer::SetClearColor(float r, float g, float b, float a) { m_clearColor = ClearColor(r, g, b, a); }

