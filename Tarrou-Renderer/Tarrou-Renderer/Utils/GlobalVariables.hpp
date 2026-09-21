//
//  GlobalVariables.hpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#include <string>
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

    static constexpr simd_float3 DEFAULT_POSITION = { 0.0f, 1.0f, -5.0f };
    static constexpr simd_float3 DEFAULT_ROTATION = { 0.0f, 0.0f, 0.0f };

    static constexpr float       GROUND_SIZE                  = 20.0f;
    static constexpr float       GROUND_HEIGHT                = 0.0f;

    static constexpr float       GROUND_TEXTURE_TILING        = 10.0f;
    static constexpr float       BUDDHA_OFFSET                = 0.45f;

    static constexpr simd_float3 LIGHT_DIR = { 0.577f, -0.577f, 0.577f };
    static constexpr simd_float4 LIGHT_DIFF = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr simd_float4 LIGHT_AMB = { 0.2f, 0.2f, 0.2f, 1.0f};
    static constexpr float       LIGHT_INTEN = 5.0f;

    static constexpr float       SHADOW_VIEW_WIDTH = 20.0f;
    static constexpr float       SHADOW_VIEW_HEIGHT = 20.0f;
    static constexpr float       SHADOW_NEAR_Z = 0.1f;
    static constexpr float       SHADOW_FAR_Z = 1000.0f;
    static constexpr float       SHADOW_MAP_WIDTH = 2048.0f;
    static constexpr float       SHADOW_MAP_HEIGHT = 2048.0f;
    static constexpr float       SHADOW_BIAS = 0.005f;
    static constexpr float       SHADOW_SPREAD = 1.0f;
} // GlobalVariables - 변수

namespace GlobalVariables {

    inline std::string GetRootPath(const std::string& relativePath) {
    #ifdef PROJECT_ROOT
    #define STR_HELPER(x) #x
    #define STR(x) STR_HELPER(x)
        static const char* kProjectRoot = STR(PROJECT_ROOT);
        std::string base(kProjectRoot);
        if (!base.empty() && (base.back() == '/' || base.back() == '\\')) {
            return base + relativePath;
        } else {
            return base + "/" + relativePath;
        }
    #else
        return relativePath;
    #endif
    } // GetAssetPath

    static constexpr const char* MODEL_RELATIVE_PATH = "Assets/buddha.obj";
    static constexpr const char* GROUND_TEXTURE_RELATIVE_PATH = "Assets/ground.png";

} // GlobalVariables - assets

namespace GlobalVariables {
    static constexpr const char* BASE_PATH = "Tarrou-Renderer/Metal/Shaders";

    static constexpr const char* DEFAULT_BUDDHA_SHADER_VERTEX   = "DefaultBuddhaVS";
    static constexpr const char* DEFAULT_BUDDHA_SHADER_FRAGMENT = "DefaultBuddhaPS";

#define MESH_SHADER_PATH "Tarrou-Renderer/Metal/Shaders"

    static constexpr const char* MESH_SHADER          = MESH_SHADER_PATH "/BuddhaMesh.metal";
    static constexpr const char* MESH_SHADER_MESH     = "BuddhaMeshMS";
    static constexpr const char* MESH_SHADER_FRAGMENT = "BuddhaMeshPS";
    static constexpr const char* MESH_SHADER_OBJECT   = "BuddhaObjectOS";

    static constexpr const char* GROUND_SHADER          = MESH_SHADER_PATH "/Ground.metal";
    static constexpr const char* GROUND_SHADER_VERTEX   = "GroundVS";
    static constexpr const char* GROUND_SHADER_FRAGMENT = "GroundPS";

    static constexpr const char* SHADOW_MESH_SHADER = MESH_SHADER_PATH "/ShadowMesh.metal";
    static constexpr const char* SHADOW_SHADER_MESH    = "ShadowMeshMS";
    static constexpr const char* SHADOW_SHADER_OBJECT   = "ShadowObjectOS";

#undef MESH_SHADER_PATH
} // GlobalVariables - Shader
