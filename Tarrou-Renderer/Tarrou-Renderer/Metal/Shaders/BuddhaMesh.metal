//
//  BuddhaMesh.metal
//  Tarrou-Renderer
//
//  Created by B0X on 9/17/26.
//

#include <metal_stdlib>
using namespace metal;

struct Meshlet {
    uint vertex_offset;
    uint triangle_offset;
    uint vertex_count;
    uint triangle_count;
};

struct CameraUniforms {
    float4x4 viewProjectionMatrix;
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

float3 GetHashColor(uint id) {
    uint hash = id * 747796405u + 2891336453u;
    hash = ((hash >> ((hash >> 28u) + 4u)) ^ hash) * 277803737u;
    hash = (hash >> 22u) ^ hash;
    
    float r = float(hash & 255) / 255.0;
    float g = float((hash >> 8) & 255) / 255.0;
    float b = float((hash >> 16) & 255) / 255.0;
    
    return float3(r, g, b);
} // GetHashColor

// 최대 정점 64개, 최대 삼각형 124개를 출력하는 Mesh 타입 정의
using mesh_t = mesh<VertexOut, void, 64, 124, topology::triangle>;

[[mesh]]
void BuddhaMeshMS(mesh_t                   output,
                  const device Meshlet*    meshlets [[buffer(0)]],
                  const device uint*       meshlet_vertices [[buffer(1)]],
                  const device uchar*      meshlet_triangles [[buffer(2)]],
                  const device VertexIn*   vertex_data [[buffer(3)]],
                  constant CameraUniforms& cam [[buffer(4)]],
                  uint                     tid [[thread_position_in_threadgroup]],
                  uint                     gid [[threadgroup_position_in_grid]]) {
    Meshlet m = meshlets[gid];

    if (tid == 0) output.set_primitive_count(m.triangle_count);
    float3 m_color = GetHashColor(gid);

    if (tid < m.vertex_count) {
        uint v_idx = meshlet_vertices[m.vertex_offset + tid];
        VertexIn v_in = vertex_data[v_idx];
            
        VertexOut v_out;
        v_out.position = cam.viewProjectionMatrix * float4(v_in.position, 1.0);
        v_out.normal = v_in.normal;
            
        // 모든 버텍스에 동일한 메쉬렛 색상 부여
        v_out.meshletColor = m_color;
            
        output.set_vertex(tid, v_out);
    }

    // 삼각형 Index 처리: 최대 124개
    if (tid < m.triangle_count) {
        uint t_idx = m.triangle_offset + (tid * 3);
            
        // 각각의 정점 인덱스를 uchar 타입으로 읽어와 개별적으로 지정
        // set_index(인덱스_버퍼_위치, 정점_인덱스_값)
        output.set_index(tid * 3,     meshlet_triangles[t_idx]);
        output.set_index(tid * 3 + 1, meshlet_triangles[t_idx + 1]);
        output.set_index(tid * 3 + 2, meshlet_triangles[t_idx + 2]);
    }
} // BuddhaMeshMS

[[fragment]]
float4 BuddhaMeshPS(VertexOut in [[stage_in]]) {
    float3 norm = normalize(in.normal);
    float light = saturate(dot(norm, float3(0.5, 0.8, -0.2))) * 0.8 + 0.2;
        
    return float4(in.meshletColor * light, 1.0);
} // BuddhaMeshPS
