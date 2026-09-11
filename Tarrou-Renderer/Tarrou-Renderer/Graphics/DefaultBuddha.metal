//
//  DefaultBuddha.metal
//  Tarrou-Renderer
//
//  Created by B0X on 9/9/26.
//

#include <metal_stdlib>
using namespace metal;

struct CameraUniforms {
    float4x4 viewProjectionMatrix;
}; // CameraUniforms

struct VertexIn {
    packed_float3 position;
    packed_float3 normal;
}; // VertexIn

struct VertexOut {
    float4 position [[position]];
    float3 normal;
}; // VertexOut

vertex VertexOut DefaultBuddhaVS(const device VertexIn* vertex_array [[buffer(0)]],
                             constant CameraUniforms& cam [[buffer(1)]],
                             uint vid [[vertex_id]])
{
    VertexOut out;
    
    float4 pos4 = float4(vertex_array[vid].position, 1.0f);
    out.position = cam.viewProjectionMatrix * pos4;
    out.normal = vertex_array[vid].normal;
    
    return out;
} // DefaultBuddhaVS

fragment float4 DefaultBuddhaPS(VertexOut in [[stage_in]])
{
    float3 norm = normalize(in.normal);
    float light = saturate(dot(norm, float3(0.5, 0.8, -0.2)));
    return float4(float3(light) * float3(0.8, 0.7, 0.6), 1.0);
} // DefaultBuddhaPS
