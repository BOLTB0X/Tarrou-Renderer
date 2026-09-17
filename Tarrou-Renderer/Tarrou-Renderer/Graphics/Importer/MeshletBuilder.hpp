//
//  MeshletBuilder.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/17/26.
//

#ifndef MeshletBuilder_hpp
#define MeshletBuilder_hpp

#include <memory>
#include "MeshData.hpp"

class Meshlet;

class MeshletBuilder {
public:
    static std::unique_ptr<Meshlet> Build(const Mesh&, void*);

private:
    static const size_t MAX_VERTICES = 64;
    static const size_t MAX_TRIANGLES = 124;
};

#endif /* MeshletBuilder_hpp */
