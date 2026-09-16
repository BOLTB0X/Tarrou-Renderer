//
//  Ground.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#ifndef Ground_hpp
#define Ground_hpp

#include <vector>
#include <memory>
#include <simd/simd.h>
#include "MetalResource.h"

class Texture;

class Ground {
public:
    Ground();
    ~Ground();

    Ground(const Ground&) = delete;
    Ground& operator=(const Ground&) = delete;

    bool Init(void*);
    void Render(void*, void*, simd_float4x4);

    float GetHeight() const;

private:
    struct Vertex {
        float position[3];
        float uv[2];
    }; // Vertex

private:
    std::vector<Vertex>        m_vertices;
    std::shared_ptr<Texture>   m_texture;
    MetalResource              m_pipelineState;
    MetalResource              m_samplerState;
    MetalResource              m_vertexBuffer;
}; // Ground

#endif /* Ground_hpp */
