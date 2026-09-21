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
    void Render(void*, void*);
    void RenderShadow(void*, void*);
    void SetInstances(void*, const std::vector<simd_float3>&);
    
private:
    Mesh                                  m_mesh;
    std::vector<std::unique_ptr<Meshlet>> m_meshletsList;
    
    MetalResource                         m_pipelineState;
    MetalResource                         m_shadowPipelineState;
    MetalResource                         m_instanceBuffer;
    uint32_t                              m_instanceCount = 0;

}; // Buddha

#endif /* Buddha_hpp */

