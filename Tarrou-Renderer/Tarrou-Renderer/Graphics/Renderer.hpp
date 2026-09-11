//
//  Renderer.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <memory>
#include "MeshData.hpp"

class Camera;

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
    void Render(void*);
    void OnResize(float, float);
    void OnGUI();
    
    ClearColor& GetClearColor();
    void        SetClearColor(float, float, float, float);

private:
    ClearColor              m_clearColor;
    void*                   m_device;
    std::unique_ptr<Camera> m_Camera;
    
    Mesh                    m_mesh;
    void*                   m_pipelineState = nullptr; // 렌더 파이프라인
    void*                   m_depthState = nullptr;    // 뎁스 스텐실
}; // Renderer

#endif /* Renderer_hpp */
