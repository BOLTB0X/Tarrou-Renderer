//
//  Ground.metal
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#include <metal_stdlib>
using namespace metal;

struct GroundCameraUniforms {
    float4x4 viewProjectionMatrix;
}; // GroundCameraUniforms

struct GroundVertexIn {
    packed_float3 position;
    packed_float2 uv;
}; // GroundVertexIn

struct GroundVertexOut {
    float4 position [[position]];
    float2 uv;
}; // GroundVertexOut

vertex GroundVertexOut GroundVS(const device GroundVertexIn*   vertex_array [[buffer(0)]],
                                constant GroundCameraUniforms& cam [[buffer(1)]],
                                uint                           vid [[vertex_id]]) {
    GroundVertexOut out;

    float4 pos4 = float4(vertex_array[vid].position, 1.0f);
    out.position = cam.viewProjectionMatrix * pos4;
    out.uv = vertex_array[vid].uv;

    return out;
} // GroundVS

fragment float4 GroundPS(GroundVertexOut  in [[stage_in]],
                         texture2d<float> groundTexture [[texture(0)]],
                         sampler          groundSampler [[sampler(0)]]) {
    return groundTexture.sample(groundSampler, in.uv);
} // GroundPS
