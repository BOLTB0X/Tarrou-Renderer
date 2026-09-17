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

std::unique_ptr<Meshlet> MeshletBuilder::Build(const Mesh& mesh, void* device) {
    if (mesh.parts.empty() || mesh.parts[0].subMeshes.empty()) {
        DebugHelper::PrintError("[MeshletBuilder] Mesh is empty.");
        return nullptr;
    }

    const auto& part = mesh.parts[0];
    const auto& submesh = part.subMeshes[0];

    void* vertexBufferHandle = part.vertexBuffer.Get();
    void* indexBufferHandle = submesh.indexBuffer.Get();

    void* vertexData = RendererBridge_GetBufferContents(vertexBufferHandle);
    void* indexData = RendererBridge_GetBufferContents(indexBufferHandle);

    if (!vertexData || !indexData) {
        DebugHelper::PrintError("[MeshletBuilder] Failed to get CPU pointer from MTLBuffer.");
        return nullptr;
    }

    // 16bit -> 32bit 변환
    std::vector<unsigned int> indices32;
    if (submesh.indexTypeBytes == 2) {
        uint16_t* indices16 = static_cast<uint16_t*>(indexData);
        indices32.assign(indices16, indices16 + submesh.indexCount);
    } else {
        uint32_t* indicesRaw = static_cast<uint32_t*>(indexData);
        indices32.assign(indicesRaw, indicesRaw + submesh.indexCount);
    }

    // 결과물을 담을 Meshlet 인스턴스 생성
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
        static_cast<const float*>(vertexData),
        part.vertexCount,
        part.vertexStride,
        MAX_VERTICES,
        MAX_TRIANGLES,
        0.0f
    );

    if (meshlet_count == 0) {
        DebugHelper::PrintError("[MeshletBuilder] buildMeshlets failed.");
        return nullptr;
    }

    // 메모리 정리 (Trim)
    const meshopt_Meshlet& last = meshletData->m_meshlets[meshlet_count - 1];
    meshletData->m_meshlets.resize(meshlet_count);
    meshletData->m_meshletVertices.resize(last.vertex_offset + last.vertex_count);
    meshletData->m_meshletTriangles.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));

    // GPU 버퍼 생성
    meshletData->m_meshletBuffer = MetalResource::Adopt(
        RendererBridge_CreateBuffer(device, meshletData->m_meshlets.data(), meshletData->m_meshlets.size() * sizeof(meshopt_Meshlet))
    );
    meshletData->m_meshletVerticesBuffer = MetalResource::Adopt(
        RendererBridge_CreateBuffer(device, meshletData->m_meshletVertices.data(), meshletData->m_meshletVertices.size() * sizeof(unsigned int))
    );
    meshletData->m_meshletTrianglesBuffer = MetalResource::Adopt(
        RendererBridge_CreateBuffer(device, meshletData->m_meshletTriangles.data(), meshletData->m_meshletTriangles.size() * sizeof(unsigned char))
    );

    if (!meshletData->m_meshletBuffer || !meshletData->m_meshletVerticesBuffer || !meshletData->m_meshletTrianglesBuffer) {
        DebugHelper::PrintError("[MeshletBuilder] Failed to create GPU buffers.");
        return nullptr;
    }

    std::cout << "[MeshletBuilder] 생성 성공! " << meshlet_count << " meshlets.\n";
    return meshletData;
} // Build
