//
//  MeshletBuilder.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/17/26.
//

#include "MeshletBuilder.hpp"
#include "Meshlet.hpp"
#include "RendererBridge.h"
#include "DebugHelper.hpp"
// STL
#include <iostream>

std::vector<std::unique_ptr<Meshlet>> MeshletBuilder::Build(const Mesh& mesh, void* device) {
    std::vector<std::unique_ptr<Meshlet>> results;

    if (mesh.parts.empty()) {
        DebugHelper::PrintError("[MeshletBuilder] Mesh is empty.");
        return results;
    }

    for (const auto& part : mesh.parts) {
        void* vertexBufferHandle = part.vertexBuffer.Get();
        auto* vertexDataRaw = static_cast<unsigned char*>(RendererBridge_GetBufferContents(vertexBufferHandle));
        if (!vertexDataRaw) continue;
        
        unsigned char* vertexData = vertexDataRaw + part.vertexBufferOffset;

        for (const auto& submesh : part.subMeshes) {
            void* indexBufferHandle = submesh.indexBuffer.Get();
            auto* indexDataRaw = static_cast<unsigned char*>(RendererBridge_GetBufferContents(indexBufferHandle));
            if (!indexDataRaw) continue;
            
            unsigned char* indexData = indexDataRaw + submesh.indexBufferOffset;

            // 16bit -> 32bit 변환
            std::vector<unsigned int> indices32;
            if (submesh.indexTypeBytes == 2) {
                uint16_t* indices16 = reinterpret_cast<uint16_t*>(indexData);
                indices32.assign(indices16, indices16 + submesh.indexCount);
            } else {
                uint32_t* indicesRaw = reinterpret_cast<uint32_t*>(indexData);
                indices32.assign(indicesRaw, indicesRaw + submesh.indexCount);
            }

            auto meshletData = std::make_unique<Meshlet>();
            size_t max_meshlets = meshopt_buildMeshletsBound(indices32.size(), MAX_VERTICES, MAX_TRIANGLES);
            meshletData->m_meshlets.resize(max_meshlets);
            meshletData->m_meshletVertices.resize(max_meshlets * MAX_VERTICES);
            meshletData->m_meshletTriangles.resize(max_meshlets * MAX_TRIANGLES * 3);

            size_t meshlet_count = meshopt_buildMeshlets(
                meshletData->m_meshlets.data(),
                meshletData->m_meshletVertices.data(),
                meshletData->m_meshletTriangles.data(),
                indices32.data(),
                indices32.size(),
                reinterpret_cast<const float*>(vertexData),
                part.vertexCount,
                part.vertexStride,
                MAX_VERTICES,
                MAX_TRIANGLES,
                0.0f
            );

            if (meshlet_count == 0) continue;

            const meshopt_Meshlet& last = meshletData->m_meshlets[meshlet_count - 1];
            meshletData->m_meshlets.resize(meshlet_count);
            meshletData->m_meshletVertices.resize(last.vertex_offset + last.vertex_count);
            meshletData->m_meshletTriangles.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));

            meshletData->m_meshletBuffer = MetalResource::Adopt(
                RendererBridge_CreateBuffer(device, meshletData->m_meshlets.data(), meshletData->m_meshlets.size() * sizeof(meshopt_Meshlet))
            );
            meshletData->m_meshletVerticesBuffer = MetalResource::Adopt(
                RendererBridge_CreateBuffer(device, meshletData->m_meshletVertices.data(), meshletData->m_meshletVertices.size() * sizeof(unsigned int))
            );
            meshletData->m_meshletTrianglesBuffer = MetalResource::Adopt(
                RendererBridge_CreateBuffer(device, meshletData->m_meshletTriangles.data(), meshletData->m_meshletTriangles.size() * sizeof(unsigned char))
            );

            results.push_back(std::move(meshletData));
        } // for (const auto& submesh : part.subMeshes)
    } // for (const auto& part : mesh.parts)

    return results;
} // Build
