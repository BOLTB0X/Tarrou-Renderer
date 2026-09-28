//
//  Frustum.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/26/26.
//

#include "Frustum.hpp"

void Frustum::Update(simd_float3 position, simd_float3 forward,
                     simd_float3 right, simd_float3 up,
                     float verticalFovRadians,
                     float aspect,
                     float nearPlane,
                     float farPlane) {
    const float tanHalfVertical = std::tan(verticalFovRadians * 0.5f);
    const float tanHalfHorizontal = tanHalfVertical * aspect;
    const float verticalLength = std::sqrt(1.0f + tanHalfVertical * tanHalfVertical);
    const float horizontalLength = std::sqrt(1.0f + tanHalfHorizontal * tanHalfHorizontal);
    const float verticalSin = tanHalfVertical / verticalLength;
    const float verticalCos = 1.0f / verticalLength;
    const float horizontalSin = tanHalfHorizontal / horizontalLength;
    const float horizontalCos = 1.0f / horizontalLength;

    SetPlane(0, forward, position + forward * nearPlane);
    SetPlane(1, -forward, position + forward * farPlane);
    SetPlane(2, simd_normalize(forward * horizontalSin + right * horizontalCos), position);
    SetPlane(3, simd_normalize(forward * horizontalSin - right * horizontalCos), position);
    SetPlane(4, simd_normalize(forward * verticalSin - up * verticalCos), position);
    SetPlane(5, simd_normalize(forward * verticalSin + up * verticalCos), position);
} // Update

bool Frustum::CheckSphere(simd_float3 center, float radius) const {
    for (const simd_float4& plane : m_planes) {
        const simd_float3 normal = simd_make_float3(plane.x, plane.y, plane.z);
        if (simd_dot(normal, center) + plane.w < -radius) {
            return false;
        }
    }
    return true;
} // CheckSphere

const simd_float4* Frustum::GetPlanes() const { return m_planes.data(); }

void Frustum::SetPlane(size_t index, simd_float3 normal, simd_float3 point) {
    normal = simd_normalize(normal);
    m_planes[index] = simd_make_float4(normal.x, normal.y, normal.z, -simd_dot(normal, point));
} // SetPlane
