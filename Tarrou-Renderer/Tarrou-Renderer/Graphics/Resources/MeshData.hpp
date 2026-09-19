//
//  Mesh.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#ifndef MESHDATA_hpp
#define MESHDATA_hpp

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include "MetalResource.h"
 
struct MeshSubmesh {
    MetalResource indexBuffer;
    size_t        indexBufferOffset = 0;
    uint32_t      indexCount = 0;
    uint32_t      indexTypeBytes = 2;
    uint32_t      primitiveType = 3;
}; // MeshSubmesh

struct MeshPart {
    MetalResource            vertexBuffer;
    size_t                   vertexBufferOffset = 0;
    uint32_t                 vertexStride = 0;
    uint32_t                 vertexCount = 0;
    std::vector<MeshSubmesh> subMeshes;
}; // MeshPart
 
struct Mesh {
    std::vector<MeshPart> parts;
}; // Mesh

#endif // !MESHDATA_hpp
