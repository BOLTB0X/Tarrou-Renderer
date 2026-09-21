//
//  ShadowMesh.metal
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#include <metal_stdlib>
#include "ShaderCommon.metal"

using namespace metal;

struct Meshlet {
    uint vertex_offset;
    uint triangle_offset;
    uint vertex_count;
    uint triangle_count;
};

struct VertexIn {
    packed_float3 position;
    packed_float3 normal;
};

struct ShadowVertexOut {
    float4 position [[position]];
};

struct ModelMatrixUniforms {
    float4x4 ModelMatrix;
};

struct ObjectPayload {
    uint meshletIndex;
    uint instanceIndex;
}; // ObjectPayload

using shadow_mesh_t = mesh<ShadowVertexOut, void, 64, 124, topology::triangle>;

[[object, max_total_threads_per_threadgroup(1)]]
void ShadowObjectOS(object_data ObjectPayload& payload [[payload]],
                    mesh_grid_properties      outGrid,
                    uint2                     tgid [[threadgroup_position_in_grid]])
{
    payload.meshletIndex  = tgid.x;
    payload.instanceIndex = tgid.y;
    outGrid.set_threadgroups_per_grid(uint3(1, 1, 1));
} // ShadowObjectOS

[[mesh]]
void ShadowMeshMS(shadow_mesh_t                     output,
                  const object_data ObjectPayload&  payload [[payload]],
                  const device Meshlet*             meshlets [[buffer(0)]],
                  constant DirectionalLightCB&      lightData [[buffer(2)]],
                  const device uint*                meshlet_vertices [[buffer(3)]],
                  const device uchar*               meshlet_triangles [[buffer(4)]],
                  const device VertexIn*            vertex_data [[buffer(5)]],
                  constant float4x4*                instances [[buffer(6)]],
                  uint                              tid [[thread_position_in_threadgroup]]) {
    uint meshletIdx = payload.meshletIndex;
    uint instanceIdx = payload.instanceIndex;

    Meshlet m = meshlets[meshletIdx];
    float4x4 modelMatrix = instances[instanceIdx];

    if (tid == 0) output.set_primitive_count(m.triangle_count);

    if (tid < m.vertex_count) {
        uint v_idx = meshlet_vertices[m.vertex_offset + tid];
        float4 worldPos = float4(vertex_data[v_idx].position, 1.0);
        
        ShadowVertexOut v_out;
        v_out.position = lightData.lightProjectionMatrix * lightData.lightViewMatrix * modelMatrix * worldPos;
        
        output.set_vertex(tid, v_out);
    }

    if (tid < m.triangle_count) {
        uint t_idx = m.triangle_offset + (tid * 3);
        output.set_index(tid * 3,     meshlet_triangles[t_idx]);
        output.set_index(tid * 3 + 1, meshlet_triangles[t_idx + 1]);
        output.set_index(tid * 3 + 2, meshlet_triangles[t_idx + 2]);
    }
} // ShadowMeshMS
