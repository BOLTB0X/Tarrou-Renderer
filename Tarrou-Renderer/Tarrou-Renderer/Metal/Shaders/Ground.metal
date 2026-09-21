//
//  Ground.metal
//  Tarrou-Renderer
//
//  Created by B0X on 9/16/26.
//

#include <metal_stdlib>
#include "ShaderCommon.metal"

using namespace metal;

struct GroundVertexIn {
    packed_float3 position;
    packed_float2 uv;
};

struct GroundVertexOut {
    float4 position [[position]];
    float3 worldPosition;
    float2 uv;
    float4 lightSpacePos;
};

vertex GroundVertexOut GroundVS(const device GroundVertexIn* vertex_array [[buffer(0)]],
                                constant FrameCB&            frameData    [[buffer(1)]],
                                constant DirectionalLightCB& lightData    [[buffer(2)]],
                                uint                         vid          [[vertex_id]]) {
    GroundVertexOut out;
    float4 pos4 = float4(vertex_array[vid].position, 1.0f);
    
    out.worldPosition = pos4.xyz;
    out.position = frameData.projection * frameData.view * pos4;
    out.uv = vertex_array[vid].uv;
    
    out.lightSpacePos = lightData.lightProjectionMatrix * lightData.lightViewMatrix * pos4;
    return out;
} // GroundVS

fragment float4 GroundPS(GroundVertexOut      in            [[stage_in]],
                         constant DirectionalLightCB& lightData [[buffer(2)]],
                         texture2d<float>     groundTexture [[texture(0)]],
                         depth2d<float>       shadowTexture [[texture(1)]],
                         sampler              groundSampler [[sampler(0)]]) {
    float4 color = groundTexture.sample(groundSampler, in.uv);

    float3 ndc = in.lightSpacePos.xyz / in.lightSpacePos.w;
    float2 shadowUV = ndc.xy * 0.5 + 0.5;

    // 필요 시 뒤집기 (상하 반전이 보이면 이 줄을 켜/끄며 테스트)
    shadowUV.y = 1.0 - shadowUV.y;

    float shadowFactor = 1.0;

    if (shadowUV.x >= 0.0 && shadowUV.x <= 1.0 && shadowUV.y >= 0.0 && shadowUV.y <= 1.0) {
        float currentDepth = ndc.z;

        // 수동 비교 시에는 nearest가 더 예측 가능
        constexpr sampler shadowSampler(coord::normalized, filter::nearest, address::clamp_to_edge);

        float shadowMapDepth = shadowTexture.sample(shadowSampler, shadowUV);

        // “수신자가 더 멀면(=가려졌다면) 그림자”
        if ((currentDepth - lightData.shadowBias) < shadowMapDepth) {
            shadowFactor = 0.4;
        }
    }

    return float4(color.rgb * shadowFactor, color.a);
}
