//
//  DirectionalLight.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#ifndef DirectionalLight_hpp
#define DirectionalLight_hpp

#include <simd/simd.h>

class DirectionalLight {
public:
    DirectionalLight();
    ~DirectionalLight();
    
    DirectionalLight(const DirectionalLight&) = delete;
    DirectionalLight& operator=(const DirectionalLight&) = delete;

    void Init();
    void Frame();
    void OnGUI();
    
public:
    // Setter
    void          SetLookAt(simd_float3);
    void          SetLookAt(float, float, float);
    // Getter
    simd_float3   GetPosition() const;
    simd_float3   GetDirection() const;
    simd_float4   GetDiffuse() const;
    simd_float4   GetAmbient() const;
    simd_float3   GetLookAt() const;
    simd_float4x4 GetViewMatrix() const;
    simd_float4x4 GetProjectionMatrix() const;

private:
    simd_float3   m_direction;
    simd_float4   m_ambient;
    simd_float4   m_diffuse;
    simd_float3   m_lookAt;
    
    simd_float4x4 m_viewMatrix;
    simd_float4x4 m_projectionMatrix;
    float         m_intensity;
};

#endif /* DirectionalLight_hpp */
