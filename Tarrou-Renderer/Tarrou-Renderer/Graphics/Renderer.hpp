//
//  Renderer.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <memory>
#include "MetalResource.h"

class Camera;
class Ground;
class Buddha;
class DirectionalLight;
class CommonConstantBuffer;

class Renderer {
public:
    struct ClearColor {
        float r = 0.05f;
        float g = 0.05f;
        float b = 0.08f;
        float a = 1;
    }; // ClearColor

    struct UpdateParam {
        float deltaTime = 0;
        float moveForward = 0;
        float moveRight = 0;
        float moveUp = 0;
        float rotationDeltaX = 0;
        float rotationDeltaY = 0;
        float zoomDelta = 0;
    }; // UpdateParam

public:
    Renderer();
    ~Renderer();

    bool Init(void*, float, float);
    void Update(const UpdateParam&);
    void Render(void*, void*);
    void OnResize(float, float);
    void OnGUI();

    ClearColor& GetClearColor();
    void        SetClearColor(float, float, float, float);

private:
    ClearColor                            m_clearColor;
    void*                                 m_device;
    std::unique_ptr<Camera>               m_Camera;
    std::unique_ptr<DirectionalLight>     m_DirLight;
    std::unique_ptr<Buddha>               m_Buddha;
    std::unique_ptr<Ground>               m_Ground;
    MetalResource                         m_depthState;
    std::unique_ptr<CommonConstantBuffer> m_CommonCB;
    
    float                                 m_globalTime = 0.0f;
    float                                 m_screenWidth = 0.0f;
    float                                 m_screenHeight = 0.0f;
    
    MetalResource                         m_shadowTexture;
    MetalResource                         m_shadowPassDescriptor;
}; // Renderer

#endif /* Renderer_hpp */
