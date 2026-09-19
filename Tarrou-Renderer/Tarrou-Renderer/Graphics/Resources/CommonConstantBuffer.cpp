//
//  CommonConstantBuffer.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#include "CommonConstantBuffer.hpp"
#include "RendererBridge.h"

CommonConstantBuffer::CommonConstantBuffer() {
    m_frameCB = {};
    m_lightCB = {};
} // CommonConstantBuffer

CommonConstantBuffer::~CommonConstantBuffer() {}

void CommonConstantBuffer::UpdateFrameCB(const FrameCB& data) { m_frameCB = data; }
void CommonConstantBuffer::UpdateLightCB(const DirectionalLightCB& data) { m_lightCB = data; }

void CommonConstantBuffer::Bind(void* renderCommandEncoder) {
    if (!renderCommandEncoder) return;
    
    RendererBridge_BindConstantBuffers(renderCommandEncoder,
                                       &m_frameCB, sizeof(FrameCB),
                                       &m_lightCB, sizeof(DirectionalLightCB));
} // Bind
