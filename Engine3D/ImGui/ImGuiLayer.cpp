#include "ImGuiLayer.h"

#include <imgui.h>
#include "../Core/DebugSettings.h"
#include "../Camera/Camera.h"

namespace
{
    void CameraBuild(Camera& InCamera)
    {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            glm::vec3 Pos = InCamera.GetPosition();
            if (ImGui::DragFloat3("Camera Position", &Pos.x, 0.5f))
            {
                InCamera.SetPosition(Pos);
            }

            float Speed = InCamera.GetSpeed();
            if (ImGui::SliderFloat("Camera Speed", &Speed, 1.0f, 50.f))
            {
                InCamera.SetSpeed(Speed);
            }

            float Sensitivity = InCamera.GetSensitivity();
            if (ImGui::SliderFloat("Sensitivity", &Sensitivity, 0.01f, 1.0f))
            {
                InCamera.SetSensitivity(Sensitivity);
            }

            float FOV = InCamera.GetFOV();
            if (ImGui::SliderFloat("FOV", &FOV, 1.0f, 90.0f))
            {
                InCamera.SetFOV(FOV);
            }

            float Pitch = InCamera.GetPitch();
            if (ImGui::SliderFloat("Pitch", &Pitch, -89.0f, 89.0f))
            {
                InCamera.SetPitch(Pitch);
            }

            float Yaw = InCamera.GetYaw();
            if (ImGui::SliderFloat("Yaw", &Yaw, -180.0f, 180.0f))
            {
                InCamera.SetYaw(Yaw);
            }
        }
    }

    void LightingBuild(DebugSettings& InSettings)
    {
        if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Animate Light", &InSettings.bAnimateLight);

            if (InSettings.bAnimateLight)
            {
                ImGui::BeginDisabled();
            }

            ImGui::DragFloat3("Light Position", &InSettings.LightPosition.x, 0.1f, -50.0f, 50.0f);

            if (InSettings.bAnimateLight)
            {
                ImGui::EndDisabled();
            }

            ImGui::SliderFloat("Light Intensity", &InSettings.LightIntensity, 0.0f, 1.0f);
        }
    }
} // namespace

namespace ImGuiLayer
{
    void BuildUI(DebugSettings& InSettings, Camera& InCamera)
    {
        ImGui::Begin("Settings");

        ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

        ImGui::Separator();
        ImGui::ColorEdit3("Background", InSettings.BackgroundColor);

        ImGui::Separator();
        ImGui::Checkbox("Wireframe", &InSettings.bWireframe);

        ImGui::Separator();
        CameraBuild(InCamera);

        ImGui::Separator();
        LightingBuild(InSettings);

        ImGui::End();
    }
} // namespace ImGuiLayer