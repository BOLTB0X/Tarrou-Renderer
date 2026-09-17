//
//  Buddha.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#ifndef Buddha_hpp
#define Buddha_hpp

#include <simd/simd.h>
#include <vector>
#include "MeshData.hpp"
#include "MetalResource.h"

class Meshlet;

class Buddha {
public:
    Buddha();
    ~Buddha();

    Buddha(const Buddha&) = delete;
    Buddha& operator=(const Buddha&) = delete;

    bool Init(void*);
    void Render(void*, void*, simd_float4x4);

    void        SetPosition(simd_float3);
    simd_float3 GetPosition() const;

private:
    Mesh                     m_mesh;
    std::unique_ptr<Meshlet> m_meshletData;
    MetalResource            m_pipelineState;
    simd_float3              m_position = { 0.0f, 0.0f, 0.0f };
}; // Buddha

#endif /* Buddha_hpp */
