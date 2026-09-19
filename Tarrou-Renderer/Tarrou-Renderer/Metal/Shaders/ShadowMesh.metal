//
//  ShadowMesh.metal
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#include <metal_stdlib>
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

struct ShadowOut {
    float4 position [[position]];
};

using shadow_mesh_t = mesh<ShadowOut, void, 64, 124, topology::triangle>;

[[mesh]]
void ShadowMeshMS(shadow_mesh_t         output,
                  const device Meshlet* meshlets [[buffer(0)]],
                  const device uint*    meshlet_vertices [[buffer(1)]],
                  const device uchar*   meshlet_triangles [[buffer(2)]],
                  const device VertexIn* vertex_data [[buffer(3)]],
                  constant float4x4&    lightSpaceMatrix [[buffer(4)]],
                  uint                  tid [[thread_position_in_threadgroup]],
                  uint                  gid [[threadgroup_position_in_grid]]) {
    Meshlet m = meshlets[gid];
    if (tid == 0) output.set_primitive_count(m.triangle_count);

    if (tid < m.vertex_count) {
        uint v_idx = meshlet_vertices[m.vertex_offset + tid];
        ShadowOut v_out;
        
        v_out.position = lightSpaceMatrix * float4(vertex_data[v_idx].position, 1.0);
        output.set_vertex(tid, v_out);
    }

    if (tid < m.triangle_count) {
        uint t_idx = m.triangle_offset + (tid * 3);
        output.set_index(tid * 3,     meshlet_triangles[t_idx]);
        output.set_index(tid * 3 + 1, meshlet_triangles[t_idx + 1]);
        output.set_index(tid * 3 + 2, meshlet_triangles[t_idx + 2]);
    }
} // ShadowMeshMS
