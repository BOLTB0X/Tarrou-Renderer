#ifndef Frustum_hpp
#define Frustum_hpp

#include <array>
#include <cmath>
#include <simd/simd.h>

class Frustum {
public:
    void               Update(simd_float3, simd_float3, simd_float3, simd_float3, float, float, float, float);
    bool               CheckSphere(simd_float3, float) const;
    const simd_float4* GetPlanes() const;


private:
    void SetPlane(size_t, simd_float3, simd_float3);

    std::array<simd_float4, 6> m_planes{};
}; // Frustum

#endif /* Frustum_hpp */
