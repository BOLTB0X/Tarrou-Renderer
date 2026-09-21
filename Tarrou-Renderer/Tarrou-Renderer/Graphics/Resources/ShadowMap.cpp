//
//  ShadowMap.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/20/26.
//

#include "ShadowMap.hpp"
#include "DebugHelper.hpp"
#include "ShadowMapBridge.h"
// STL
#include <iostream>

ShadowMap::ShadowMap()
    : m_depthTexture(nullptr), m_renderPassDescriptor(nullptr), m_width(0), m_height(0) {}

ShadowMap::~ShadowMap() {
    if (m_depthTexture) {
        ShadowMapBridge_ReleaseMetalObject(m_depthTexture);
        m_depthTexture = nullptr;
    }
    if (m_renderPassDescriptor) {
        ShadowMapBridge_ReleaseMetalObject(m_renderPassDescriptor);
        m_renderPassDescriptor = nullptr;
    }
} // ~ShadowMap

bool ShadowMap::Init(void* device, int width, int height) {
    m_width = width;
    m_height = height;

    m_depthTexture = ShadowMapBridge_CreateShadowDepthTexture(device, width, height);
    if (!m_depthTexture) {
        DebugHelper::PrintError("[ShadowMap] Failed to create Depth Texture.");
        return false;
    }

    m_renderPassDescriptor = ShadowMapBridge_CreateShadowPassDescriptor(m_depthTexture);
    if (!m_renderPassDescriptor) {
        DebugHelper::PrintError("[ShadowMap] Failed to create Render Pass Descriptor.");
        return false;
    }

    std::cout << "[ShadowMap] Successfully initialized (" << width << "x" << height << ")" << std::endl;
    return true;
} // Init

void* ShadowMap::GetDepthTexture() const { return m_depthTexture; }
void* ShadowMap::GetRenderPassDescriptor() const { return m_renderPassDescriptor; }
int   ShadowMap::GetWidth() const { return m_width; }
int   ShadowMap::GetHeight() const { return m_height; }
