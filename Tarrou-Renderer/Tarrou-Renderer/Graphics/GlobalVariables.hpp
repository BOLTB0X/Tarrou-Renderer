//
//  GlobalVariables.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#include <simd/simd.h>

namespace GlobalVariables {
    static constexpr int         SCREEN_WIDTH = 800;
    static constexpr int         SCREEN_HEIGHT = 600;
    static constexpr float       SCREEN_DEPTH = 1000.0f;
    static constexpr float       SCREEN_NEAR = 0.1f;
    static constexpr float       SCREEN_ASPECT = 16.0f / 9.0f;

    static constexpr float       MAX_PITCH = 89.0f;
    static constexpr float       MIN_PITCH = -89.0f;
    static constexpr float       MIN_FOV = 1.0f;
    static constexpr float       MAX_FOV = 129.0f;
    static constexpr float       DEFAULT_FOV = 60.0f;

    static constexpr simd_float3 DEFAULT_POSITION = { 0.0f, 10.0f, -5.0f };
    static constexpr simd_float3 DEFAULT_ROTATION = { 0.0f, 0.0f, 0.0f };

} // GlobalVariables
