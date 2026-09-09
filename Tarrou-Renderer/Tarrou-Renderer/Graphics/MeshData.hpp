//
//  Mesh.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#ifndef MESHDATA_hpp
#define MESHDATA_hpp
#include <cstdint>
#include <vector>
#include <string>

struct MeshSubmesh {
    void*    indexBuffer = nullptr;    // id<MTLBuffer>
    uint32_t indexCount = 0;
    uint32_t indexTypeBytes = 2;       // 2=uint16, 4=uint32
    uint32_t primitiveType = 3;        // MTLPrimitiveTypeTriangle = 3
}; // MeshSubmesh

struct MeshPart {
    void*                    vertexBuffer = nullptr;   // id<MTLBuffer>
    uint32_t                 vertexStride = 0;
    uint32_t                 vertexCount = 0;
    std::vector<MeshSubmesh> subMeshes;
}; // MeshPart

struct Mesh {
    std::vector<MeshPart> parts;
};

#endif // !MESHDATA_hpp
