//
//  DirectionalLight.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/18/26.
//

#include "DirectionalLight.hpp"
#include "MathHelper.hpp"
#include "GlobalVariables.hpp"
#include "imgui.h"
// STL
#include <cmath>

using namespace GlobalVariables;

DirectionalLight::DirectionalLight()
    : m_direction{LIGHT_DIR}, m_diffuse{LIGHT_DIFF},
    m_ambient{LIGHT_AMB}, m_intensity(LIGHT_INTEN), m_lookAt{0.0f, 0.0f, 0.0f} {
    m_viewMatrix = matrix_identity_float4x4;
    m_projectionMatrix = matrix_identity_float4x4;
} // DirectionalLight

DirectionalLight::~DirectionalLight() {}

void DirectionalLight::Init() {
    m_direction = simd_normalize(simd_make_float3(LIGHT_DIR));
    m_diffuse = LIGHT_DIFF;
    m_ambient = LIGHT_AMB;
    Frame();
} // Init

void DirectionalLight::Frame() {
    float length = simd_length(m_direction);
    if (length <= 0.0001f) {
        m_direction = simd_normalize(simd_make_float3(LIGHT_DIR));
    } else {
        m_direction = simd_normalize(m_direction);
    }

    simd_float3 upVector = {0.0f, 1.0f, 0.0f};
    simd_float3 lightPos = m_lookAt - (m_direction * 100.0f);

    // 방향이 거의 수직(Y축)일 경우 Up Vector 보정
    if (std::abs(m_direction.y) > 0.999f) {
        upVector = {0.0f, 0.0f, 1.0f};
    }

    m_viewMatrix = MathHelper::MatrixLookAtLH(lightPos, m_lookAt, upVector);
    m_projectionMatrix = MathHelper::MatrixOrthographicLH(SHADOW_VIEW_WIDTH, SHADOW_VIEW_HEIGHT, SHADOW_NEAR_Z, SHADOW_FAR_Z);
} // Frame

void          DirectionalLight::SetLookAt(simd_float3 lookAt) { m_lookAt = lookAt; }
void          DirectionalLight::SetLookAt(float x, float y, float z) { m_lookAt = {x, y, z}; }
simd_float3   DirectionalLight::GetPosition() const { float distance = 50.0f; return m_lookAt - (m_direction * distance); }
simd_float3   DirectionalLight::GetDirection() const { return m_direction; }
simd_float4   DirectionalLight::GetDiffuse() const { return {m_diffuse.x * m_intensity, m_diffuse.y * m_intensity, m_diffuse.z * m_intensity, m_diffuse.w}; }
simd_float4   DirectionalLight::GetAmbient() const { return m_ambient; }
simd_float3   DirectionalLight::GetLookAt() const { return m_lookAt; }
simd_float4x4 DirectionalLight::GetViewMatrix() const { return m_viewMatrix; }
simd_float4x4 DirectionalLight::GetProjectionMatrix() const { return m_projectionMatrix; }

void DirectionalLight::OnGUI() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

    if (ImGui::Button("Reset to Default", ImVec2(-1, 0))) {
        Init();
        m_intensity = LIGHT_INTEN;
    }

    ImGui::PopStyleColor(3);
    ImGui::Separator();

    if (ImGui::CollapsingHeader("LIGHT SETTINGS", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();

        // simd_float3는 메모리 구조상 float 배열과 호환되지만, 안전을 위해 임시 배열 사용
        float dir[3] = {m_direction.x, m_direction.y, m_direction.z};
        if (ImGui::SliderFloat3("Direction", dir, -1.0f, 1.0f)) {
            m_direction = {dir[0], dir[1], dir[2]};
            Frame();
        }
        
        ImGui::Spacing();
        ImGui::Separator();

        float look[3] = {m_lookAt.x, m_lookAt.y, m_lookAt.z};
        if (ImGui::DragFloat3("Look At", look, 0.1f)) {
            m_lookAt = {look[0], look[1], look[2]};
            Frame();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.8f, 1.0f), "[ Light Colors & HDR ]");
        ImGui::DragFloat("Intensity Multiplier", &m_intensity, 0.1f, 0.0f, 50.0f, "%.2f");

        ImGuiColorEditFlags hdrFlags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float;
        float dif[4] = {m_diffuse.x, m_diffuse.y, m_diffuse.z, m_diffuse.w};
        if (ImGui::ColorEdit4("Diffuse Base", dif, hdrFlags)) m_diffuse = {dif[0], dif[1], dif[2], dif[3]};
        
        float amb[4] = {m_ambient.x, m_ambient.y, m_ambient.z, m_ambient.w};
        if (ImGui::ColorEdit4("Ambient Base", amb, hdrFlags)) m_ambient = {amb[0], amb[1], amb[2], amb[3]};
    }
} // OnGUI
