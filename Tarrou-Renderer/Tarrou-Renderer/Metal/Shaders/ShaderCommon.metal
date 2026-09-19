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

#endif /* ShaderCommon_h */
