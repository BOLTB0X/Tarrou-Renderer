//
//  ShaderCommon.metal
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#include <metal_stdlib>
using namespace metal;


#ifndef ShaderCommon_h
#define ShaderCommon_h

#include <metal_stdlib>
using namespace metal;

struct FrameCB {
    float4x4 view;
    float4x4 projection;
    float4x4 viewInv;
    float4x4 projInv;
    
    float3   cameraPosition;
    float    cameraFov;
    
    float2   screenResolution;
    float    time;
    float    fPadding;
}; // FrameCB

struct DirectionalLightCB {
    float3   lightDir;
    float    dPadding1;
    
    float4   lightAmbient;
    float4   lightDiffuse;
    
    float3   lightLookAt;
    float    dPadding2;
    
    float4x4 lightViewMatrix;
    float4x4 lightProjectionMatrix;
    
    float    shadowMapWidth;
    float    shadowMapHeight;
    float    shadowBias;
    float    shadowSpread;
}; // DirectionalLightCB

inline float CalculateShadowPCF(depth2d<float> shadowTexture,
                                sampler        shadowSampler,
                                float2         shadowUV,
                                float          currentDepth,
                                float2         texelSize,
                                float          shadowBias,
                                bool           reverseZ,
                                int            kernelRadius) {
    float occlusion = 0.0;
    int sampleCount = 0;

    for (int y = -kernelRadius; y <= kernelRadius; ++y) {
        for (int x = -kernelRadius; x <= kernelRadius; ++x) {
            float2 offsetUV = shadowUV + float2(x, y) * texelSize;
            float sampledDepth = shadowTexture.sample(shadowSampler, offsetUV);

            bool isOccluded = reverseZ
                ? sampledDepth > (currentDepth + shadowBias)
                : sampledDepth < (currentDepth - shadowBias);

            if (isOccluded) { occlusion += 1.0; }
            sampleCount++;
        } // for (int x = -kernelRadius; x <= kernelRadius; ++x)
    } // for (int y = -kernelRadius; y <= kernelRadius; ++y)

    return occlusion / float(sampleCount);
} // CalculateShadowPCF

#endif /* ShaderCommon_h */
