//
//  CommonConstantBuffer.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#ifndef CommonConstantBuffer_hpp
#define CommonConstantBuffer_hpp

#include <simd/simd.h>

class CommonConstantBuffer {
public:
    struct FrameCB {
        simd_float4x4 view;
        simd_float4x4 projection;
        simd_float4x4 viewInv;
        simd_float4x4 projInv;
        
        simd_float3   cameraPosition;
        float         cameraFov;
        
        simd_float2   screenResolution;
        float         time;
        float         fPadding;
    }; // FrameCB
    
    struct DirectionalLightCB {
        simd_float3   lightDir;
        float         dPadding1;
        
        simd_float4   lightAmbient;
        simd_float4   lightDiffuse;
        
        simd_float3   lightLookAt;
        float         dPadding2;
        
        simd_float4x4 lightViewMatrix;
        simd_float4x4 lightProjectionMatrix;
        
        float         shadowMapWidth;
        float         shadowMapHeight;
        float         shadowBias;
        float         shadowSpread;
    }; // DirectionalLightCB
    
public:
    CommonConstantBuffer();
    ~CommonConstantBuffer();

    void UpdateFrameCB(const FrameCB&);
    void UpdateLightCB(const DirectionalLightCB&);
    void Bind(void*);

private:
    FrameCB            m_frameCB;
    DirectionalLightCB m_lightCB;
}; // CommonConstantBuffer


#endif /* CommonConstantBuffer_hpp */
