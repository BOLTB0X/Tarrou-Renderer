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
        float r;
        float g;
        float b;
        float a;
        
        ClearColor() : r(0.05f), g(0.05f), b(0.08f), a(1.0f) {}
        ClearColor(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
    }; // ClearColor
    
public:
    Renderer();
    ~Renderer();

    bool Init(void*, float, float);
    void Update(float);
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
