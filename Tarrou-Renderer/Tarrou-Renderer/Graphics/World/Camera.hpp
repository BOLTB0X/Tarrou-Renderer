//
//  Camera.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <simd/simd.h>

class Camera {
public:
    struct InitParam {
        float fov;
        float aspect;
        float screenNear;
        float screenFar;
        
        InitParam();
    };
    
    struct FrameParam {
        float moveForward;
        float moveRight;
        float moveUp;
        float rotationDeltaX;
        float rotationDeltaY;
        float zoomDelta;
        
        FrameParam();
    };
    
public:
    Camera();
    ~Camera();

    void Init(const InitParam&);
    void Frame(const FrameParam&);
    void Update();
    void OnGUI();

public:
    // 조작 함수
    void AddRotation(float, float);
    void AddPitch(float);
    void AddYaw(float);
    void AddFOV(float);
    void Reset();

    void MoveForwardBack(float);
    void MoveLeftRight(float);
    void MoveUpDown(float);

public:
    // Setter & Getter
    void          SetPosition(const simd_float3&);
    void          SetPosition(float, float, float);
    void          SetRotation(const simd_float3&);
    void          SetRotation(float, float, float);
    void          SetFov(float);
    void          SetAspect(float);
    void          SetNear(float);
    void          SetFar(float);

    float         GetFov() const;
    float         GetNear() const;
    float         GetFar() const;
    float         GetAspect() const;
    simd_float3   GetPosition() const;
    simd_float3   GetRotation() const;
    simd_float4x4 GetViewMatrix() const;
    simd_float4x4 GetReverseZProjectionMatrix() const;
    simd_float4x4 GetStandardZProjectionMatrix() const;
    simd_float3   GetForwardVector() const;
    simd_float3   GetRightVector() const;
    simd_float3   GetUpVector() const;

private:
    void UpdateProjection();

private:
    simd_float3   m_position;
    simd_float3   m_rotation; // Pitch (X), Yaw (Y), Roll (Z) in Degrees
    simd_float4x4 m_viewMatrix;
    simd_float4x4 m_reverseProjectionMatrix;         // Reverse-Z Projection
    simd_float4x4 m_standardProjectionMatrix;        // Standard Z Projection

    simd_float3   m_forward;
    simd_float3   m_right;
    simd_float3   m_upVector;

    float         m_fov, m_near, m_far, m_aspect;
    float         m_maxPitch, m_minPitch;
    float         m_maxFov, m_minFov;
    float         m_rotationSpeed;
    float         m_moveSpeed;
    float         m_zoomSpeed;
}; // Camera

#endif /* Camera_hpp */
