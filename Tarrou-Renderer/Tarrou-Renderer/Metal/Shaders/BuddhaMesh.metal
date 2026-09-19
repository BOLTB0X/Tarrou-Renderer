//
//  BuddhaMesh.metal
//  Tarrou-Renderer
//
//  Created by B0X on 9/17/26.
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

struct VertexOut {
    float4 position [[position]];
    float3 normal;
    float3 meshletColor [[flat]];
};

struct ModelMatrixUniforms {
    float4x4 ModelMatrix;
};

struct ObjectPayload {
    uint meshletIndex;
    uint instanceIndex;
}; // ObjectPayload

float3 GetHashColor(uint id) {
    uint hash = id * 747796405u + 2891336453u;
    hash = ((hash >> ((hash >> 28u) + 4u)) ^ hash) * 277803737u;
    hash = (hash >> 22u) ^ hash;
    
    float r = float(hash & 255) / 255.0;
    float g = float((hash >> 8) & 255) / 255.0;
    float b = float((hash >> 16) & 255) / 255.0;
    
    return float3(r, g, b);
}

using mesh_t = mesh<VertexOut, void, 64, 124, topology::triangle>;

[[object, max_total_threads_per_threadgroup(1)]]
void BuddhaObjectOS(object_data ObjectPayload& payload [[payload]],
                    mesh_grid_properties      outGrid,
                    uint2                     tgid [[threadgroup_position_in_grid]])
{
    payload.meshletIndex  = tgid.x;
    payload.instanceIndex = tgid.y;
    outGrid.set_threadgroups_per_grid(uint3(1, 1, 1));
} // BuddhaObjectOS

[[mesh]]
void BuddhaMeshMS(mesh_t                            output,
                  const object_data ObjectPayload&  payload [[payload]],
                  const device Meshlet*             meshlets [[buffer(0)]],
                  constant FrameCB&                 frameData [[buffer(1)]],
                  const device uint*                meshlet_vertices [[buffer(3)]],
                  const device uchar*               meshlet_triangles [[buffer(4)]],
                  const device VertexIn*            vertex_data [[buffer(5)]],
                  constant ModelMatrixUniforms*     instances [[buffer(6)]],
                  uint                              tid [[thread_position_in_threadgroup]],
                  uint3                             ms_gid [[threadgroup_position_in_grid]]) {
    uint meshletIdx = payload.meshletIndex;
    uint instanceIdx = payload.instanceIndex;

    Meshlet m = meshlets[meshletIdx];
    float4x4 modelMatrix = instances[instanceIdx].ModelMatrix;

    if (tid == 0) output.set_primitive_count(m.triangle_count);
    float3 m_color = GetHashColor(meshletIdx);

    if (tid < m.vertex_count) {
        uint v_idx = meshlet_vertices[m.vertex_offset + tid];
        VertexIn v_in = vertex_data[v_idx];

        VertexOut v_out;
        float4 worldPos = float4(v_in.position, 1.0);
        v_out.position = frameData.projection * frameData.view * modelMatrix * worldPos;
        v_out.normal = v_in.normal;
        v_out.meshletColor = m_color;

        output.set_vertex(tid, v_out);
    }

    if (tid < m.triangle_count) {
        uint t_idx = m.triangle_offset + (tid * 3);
        output.set_index(tid * 3,     meshlet_triangles[t_idx]);
        output.set_index(tid * 3 + 1, meshlet_triangles[t_idx + 1]);
        output.set_index(tid * 3 + 2, meshlet_triangles[t_idx + 2]);
    }
} // BuddhaMeshMS

[[fragment]]
float4 BuddhaMeshPS(VertexOut                    in [[stage_in]],
                    constant DirectionalLightCB& lightData [[buffer(2)]]) {
    float3 norm = normalize(in.normal);
    float light = saturate(dot(norm, lightData.lightDir)) * 0.8 + 0.2;

    return float4(in.meshletColor * light, 1.0);
} // BuddhaMeshPS
