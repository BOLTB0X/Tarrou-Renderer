//
//  Meshlet.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/17/26.
//

#ifndef Meshlet_hpp
#define Meshlet_hpp

#include <cstdint>
#include <vector>
#include "meshoptimizer.h"
#include "MetalResource.h"

struct MeshletBounds {
    float center[3];
    float radius;
    float coneApex[3];
    float coneAxis[3];
    float coneCutoff;
    int8_t coneAxisS8[3];
    int8_t coneCutoffS8;
};

class Meshlet {
public:
    Meshlet();
    ~Meshlet();

    Meshlet(const Meshlet&) = delete;
    Meshlet& operator=(const Meshlet&) = delete;
    Meshlet(Meshlet&&) = default;
    Meshlet& operator=(Meshlet&&) = default;

public:
    // CPU
    std::vector<meshopt_Meshlet> m_meshlets;
    std::vector<unsigned int>    m_meshletVertices;
    std::vector<unsigned char>   m_meshletTriangles;
    std::vector<MeshletBounds>   m_meshletBounds;
    // GPU
    MetalResource                m_meshletBuffer;
    MetalResource                m_meshletVerticesBuffer;
    MetalResource                m_meshletTrianglesBuffer;
    MetalResource                m_meshletBoundsBuffer;
}; // Meshlet

#endif /* Meshlet_hpp */
