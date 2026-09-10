//
//  Camera.cpp
//  Tarrou-Renderer
//
//  Created by B0X on 9/8/26.
//

#include "Camera.hpp"
#include "GlobalVariables.hpp"
#include "MathHelper.hpp"
#include "imgui.h"

#include <cmath>
#include <algorithm>

using namespace GlobalVariables;
using namespace MathHelper;

Camera::InitParam::InitParam()
    : fov(DEFAULT_FOV), aspect(SCREEN_ASPECT), screenNear(SCREEN_NEAR), screenFar(SCREEN_DEPTH) {
} // InitParam

Camera::FrameParam::FrameParam()
    : moveForward(0.0f), moveRight(0.0f), moveUp(0.0f),
      rotationDeltaX(0.0f), rotationDeltaY(0.0f), zoomDelta(0.0f) {
} // FrameParam

Camera::Camera() : m_position(DEFAULT_POSITION), m_rotation(DEFAULT_ROTATION),
    m_fov(60.0f), m_near(0.1f), m_far(1000.0f), m_aspect(16.0f / 9.0f) {
    m_maxPitch = MAX_PITCH;
    m_minPitch = MIN_PITCH;
    m_maxFov = MAX_FOV;
    m_minFov = MIN_FOV;
    m_viewMatrix = matrix_identity_float4x4;
    m_reverseProjectionMatrix = matrix_identity_float4x4;
    m_standardProjectionMatrix = matrix_identity_float4x4;
    m_forward = simd_make_float3(0.0f, 0.0f, MathHelper::FRONT_Z);
    m_right = simd_make_float3(1.0f, 0.0f, 0.0f);
    m_upVector = simd_make_float3(0.0f, 1.0f, 0.0f);
    
    m_rotationSpeed = 0.05f;
    m_moveSpeed = 1.0f;
    m_zoomSpeed = 1.0f;
} // Camera

Camera::~Camera() {}

void Camera::Init(const InitParam& param) {
    m_fov = param.fov;
    m_aspect = param.aspect;
    m_near = param.screenNear;
    m_far = param.screenFar;

    UpdateProjection();
    Update();
} // Init

void Camera::Frame(const FrameParam& param) {
    if (param.rotationDeltaX != 0.0f) { AddYaw(param.rotationDeltaX * m_rotationSpeed); }
    if (param.rotationDeltaY != 0.0f) { AddPitch(param.rotationDeltaY * m_rotationSpeed); }

    if (param.moveForward != 0.0f) { MoveForwardBack(param.moveForward * m_moveSpeed); }
    if (param.moveRight != 0.0f)   { MoveLeftRight(param.moveRight * m_moveSpeed); }
    if (param.moveUp != 0.0f)      { MoveUpDown(param.moveUp * m_moveSpeed); }

    if (param.zoomDelta != 0.0f) {
        float fovDelta = -param.zoomDelta * 0.05f;
        AddFOV(fovDelta * m_zoomSpeed);
    }

    Update();
} // Frame

void Camera::UpdateProjection() {
    float fovRadian = ToRadians(m_fov);
    m_reverseProjectionMatrix = MatrixPerspectiveFovLH(fovRadian, m_aspect, m_far, m_near);
    m_standardProjectionMatrix = MatrixPerspectiveFovLH(fovRadian, m_aspect, m_near, m_far);
} // UpdateProjection

void Camera::Update() {
    simd_float4x4 rotationMatrix = MatrixRotationRollPitchYaw(
        ToRadians(m_rotation.x),
        ToRadians(m_rotation.y),
        ToRadians(m_rotation.z)
    );

    // 바라보는 방향 계산
    simd_float4 fwd4 = simd_mul(rotationMatrix, simd_make_float4(0.0f, 0.0f, FRONT_Z, 0.0f));
    simd_float4 up4  = simd_mul(rotationMatrix, simd_make_float4(0.0f, 1.0f, 0.0f, 0.0f));

    m_forward  = simd_normalize(simd_make_float3(fwd4.x, fwd4.y, fwd4.z));
    m_upVector = simd_normalize(simd_make_float3(up4.x, up4.y, up4.z));
    m_right    = simd_normalize(simd_cross(m_upVector, m_forward));

    simd_float3 lookAt = m_position + m_forward;
    m_viewMatrix = MatrixLookAtLH(m_position, lookAt, m_upVector);
    
    UpdateProjection();
} // Update

void Camera::Reset() {
    m_position = DEFAULT_POSITION;
    m_rotation = DEFAULT_ROTATION;
    m_fov = DEFAULT_FOV;
    UpdateProjection();
} // Reset

void          Camera::AddRotation(float pitch, float yaw) { AddPitch(pitch); AddYaw(yaw); }
void          Camera::AddPitch(float pitch) { m_rotation.x += pitch; m_rotation.x = std::clamp(m_rotation.x, m_minPitch, m_maxPitch); }
void          Camera::AddYaw(float yaw) { m_rotation.y += yaw; m_rotation.y = WrapAngle(m_rotation.y); }
void          Camera::AddFOV(float fovDelta) { m_fov += fovDelta; m_fov = std::clamp(m_fov, m_minFov, m_maxFov); UpdateProjection(); } // AddFOV
void          Camera::MoveForwardBack(float distance) { m_position += GetForwardVector() * distance; }
void          Camera::MoveLeftRight(float distance) { m_position += GetRightVector() * distance; }
void          Camera::MoveUpDown(float distance) { m_position += GetUpVector() * distance; }

// Setters
void          Camera::SetPosition(const simd_float3& pos) { m_position = pos; }
void          Camera::SetPosition(float x, float y, float z) { m_position = simd_make_float3(x, y, z); }
void          Camera::SetRotation(const simd_float3& rot) { m_rotation = rot; }
void          Camera::SetRotation(float x, float y, float z) { m_rotation = simd_make_float3(x, y, z); }
void          Camera::SetFov(float fov) { m_fov = fov; UpdateProjection(); }
void          Camera::SetAspect(float aspect) { m_aspect = aspect; UpdateProjection(); }
void          Camera::SetNear(float screenNear) { m_near = screenNear; UpdateProjection(); }
void          Camera::SetFar(float screenFar) { m_far = screenFar; UpdateProjection(); }
// Getters
simd_float3   Camera::GetPosition() const { return m_position; }
simd_float3   Camera::GetRotation() const { return m_rotation; }
simd_float4x4 Camera::GetViewMatrix() const { return m_viewMatrix; }
simd_float4x4 Camera::GetReverseZProjectionMatrix() const { return m_reverseProjectionMatrix; }
simd_float4x4 Camera::GetStandardZProjectionMatrix() const { return m_standardProjectionMatrix; }
float         Camera::GetFov() const { return m_fov; }
float         Camera::GetNear() const { return m_near; }
float         Camera::GetFar() const { return m_far; }
float         Camera::GetAspect() const { return m_aspect; }
simd_float3   Camera::GetForwardVector() const { return m_forward; }
simd_float3   Camera::GetRightVector() const { return m_right; }
simd_float3   Camera::GetUpVector() const { return m_upVector; }

void Camera::OnGUI() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

    if (ImGui::Button("Reset to Default", ImVec2(-1, 0))) {
        Reset();
    }
    ImGui::PopStyleColor(3);
    ImGui::Separator();

    float pos[3] = { m_position.x, m_position.y, m_position.z };
    if (ImGui::SliderFloat3("Position", pos, -100.0f, 100.0f, "%.2f")) {
        SetPosition(pos[0], pos[1], pos[2]);
    }

    float rot[3] = { m_rotation.x, m_rotation.y, m_rotation.z };
    if (ImGui::SliderFloat3("Rotation", rot, -360.0f, 360.0f, "%.1f deg")) {
        SetRotation(rot[0], rot[1], rot[2]);
    }

    ImGui::Separator();

    float fov = GetFov();
    if (ImGui::SliderFloat("FOV", &fov, 10.0f, 120.0f, "%.1f deg")) {
        SetFov(fov);
    }

    ImGui::Text("Near: %.2f / Far: %.2f", GetNear(), GetFar());

    ImGui::Separator();
    ImGui::Text("Camera Speeds & Ranges");

    static float moveSpeedMin = 0.01f;
    static float moveSpeedMax = 100.0f;
    ImGui::SliderFloat("Move Speed", &m_moveSpeed, moveSpeedMin, moveSpeedMax, "%.2f");

    ImGui::Spacing();

    static float rotSpeedMin = 0.001f;
    static float rotSpeedMax = 5.0f;
    ImGui::SliderFloat("Rotation Speed", &m_rotationSpeed, rotSpeedMin, rotSpeedMax, "%.3f");

    ImGui::Spacing();

    static float zoomSpeedMin = 0.01f;
    static float zoomSpeedMax = 50.0f;
    ImGui::SliderFloat("Zoom Speed", &m_zoomSpeed, zoomSpeedMin, zoomSpeedMax, "%.2f");

    Update();
} // OnGUI
