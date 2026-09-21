//
//  ShadowMap.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/20/26.
//

#ifndef ShadowMap_hpp
#define ShadowMap_hpp

#include <stdio.h>
#include <cstdint>

class ShadowMap {
public:
    ShadowMap();
    ~ShadowMap();
    
    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    bool Init(void*, int, int);
    
public:
    void* GetDepthTexture() const;
    void* GetRenderPassDescriptor() const;
    int   GetWidth() const;
    int   GetHeight() const;

private:
    void* m_depthTexture;
    void* m_renderPassDescriptor;
    int   m_width;
    int   m_height;
}; // ShadowMap

#endif /* ShadowMap_hpp */
