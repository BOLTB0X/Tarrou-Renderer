//
//  MathHelper.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

namespace MathHelper {
    constexpr float FRONT_Z = 1.0f; // 왼손 좌표계 전방 벡터 (+Z)

    inline float ToRadians(float deg) {
        return deg * (3.14159265358979323846f / 180.0f);
    } // ToRadians

    inline float WrapAngle(float angle) {
        angle = fmodf(angle, 360.0f);
        if (angle < 0.0f) angle += 360.0f;
        return angle;
    } // WrapAngle

    // Roll-Pitch-Yaw 회전 행렬 생성 (Column-Major)
    inline simd_float4x4 MatrixRotationRollPitchYaw(float pitchRad, float yawRad, float rollRad) {
        float cp = cosf(pitchRad), sp = sinf(pitchRad);
        float cy = cosf(yawRad),   sy = sinf(yawRad);
        float cr = cosf(rollRad),  sr = sinf(rollRad);

        simd_float4 col0 = simd_make_float4(cy * cr + sy * sp * sr,  sr * cp, -sy * cr + cy * sp * sr, 0.0f);
        simd_float4 col1 = simd_make_float4(-cy * sr + sy * sp * cr, cr * cp,  sr * sy + cy * sp * cr, 0.0f);
        simd_float4 col2 = simd_make_float4(sy * cp,                -sp,       cy * cp,                 0.0f);
        simd_float4 col3 = simd_make_float4(0.0f,                   0.0f,      0.0f,                    1.0f);

        return simd_matrix(col0, col1, col2, col3);
    } // MatrixRotationRollPitchYaw

    // LookAt 행렬 (Left-Handed)
    inline simd_float4x4 MatrixLookAtLH(simd_float3 eye, simd_float3 target, simd_float3 up) {
        simd_float3 zaxis = simd_normalize(target - eye);
        simd_float3 xaxis = simd_normalize(simd_cross(up, zaxis));
        simd_float3 yaxis = simd_cross(zaxis, xaxis);

        simd_float4 col0 = simd_make_float4(xaxis.x, yaxis.x, zaxis.x, 0.0f);
        simd_float4 col1 = simd_make_float4(xaxis.y, yaxis.y, zaxis.y, 0.0f);
        simd_float4 col2 = simd_make_float4(xaxis.z, yaxis.z, zaxis.z, 0.0f);
        simd_float4 col3 = simd_make_float4(-simd_dot(xaxis, eye), -simd_dot(yaxis, eye), -simd_dot(zaxis, eye), 1.0f);

        return simd_matrix(col0, col1, col2, col3);
    } // MatrixLookAtLH

    // Perspective Projection (Left-Handed, Metal Depth [0, 1])
    inline simd_float4x4 MatrixPerspectiveFovLH(float fovRad, float aspect, float nearZ, float farZ) {
        float h = 1.0f / tanf(fovRad * 0.5f);
        float w = h / aspect;
        float q = farZ / (farZ - nearZ);

        simd_float4 col0 = simd_make_float4(w,    0.0f, 0.0f,         0.0f);
        simd_float4 col1 = simd_make_float4(0.0f, h,    0.0f,         0.0f);
        simd_float4 col2 = simd_make_float4(0.0f, 0.0f, q,            1.0f);
        simd_float4 col3 = simd_make_float4(0.0f, 0.0f, -q * nearZ,   0.0f);

        return simd_matrix(col0, col1, col2, col3);
    } // MatrixPerspectiveFovLH
} // MathHelper
