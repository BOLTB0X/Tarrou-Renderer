//
//  MetalResource.mm
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#include "MetalResource.h"
#import <CoreFoundation/CoreFoundation.h>

MetalResource::MetalResource(void* handle)
    : m_handle(handle) {
} // MetalResource

MetalResource::~MetalResource() {
    Release();
} // ~MetalResource

MetalResource MetalResource::Adopt(void* handle) {
    return MetalResource(handle);
} // Adopt

MetalResource::MetalResource(MetalResource&& other) noexcept
    : m_handle(other.m_handle) {
    other.m_handle = nullptr;
} // MetalResource

MetalResource& MetalResource::operator=(MetalResource&& other) noexcept {
    if (this != &other) {
        Release();
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }
    return *this;
} // operator=

void MetalResource::Reset() {
    Release();
} // Reset

void MetalResource::Release() {
    if (m_handle) {
        CFRelease(static_cast<CFTypeRef>(m_handle));
        m_handle = nullptr;
    }
} // Release
