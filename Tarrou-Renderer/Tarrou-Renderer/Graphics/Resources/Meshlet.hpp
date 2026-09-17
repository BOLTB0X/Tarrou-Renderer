//
//  Meshlet.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/17/26.
//

#ifndef Meshlet_hpp
#define Meshlet_hpp

#include <vector>
#include "meshoptimizer.h"
#include "MetalResource.h"

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
    // GPU
    MetalResource                m_meshletBuffer;
    MetalResource                m_meshletVerticesBuffer;
    MetalResource                m_meshletTrianglesBuffer;
}; // Meshlet

#endif /* Meshlet_hpp */
