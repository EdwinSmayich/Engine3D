#include "ImGuiLayer.h"

#include <imgui.h>
#include "../Core/AppContext.h"

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
            if (ImGui::SliderFloat("Camera Speed", &Speed, 0.5f, 100.f))
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

            if (ImGui::Button("Reset Camera", ImVec2(-1.0f, 0.0f)))
            {
                InCamera.ResetToDefaults();
            }
        }
    }

    void LightsBuild(AppContext& InContext)
    {
        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BeginChild("Light Objects", ImVec2(0.0f, 100.0f), ImGuiChildFlags_Borders);

            if (ImGui::BeginTable("LightsTable", 3, ImGuiTableFlags_BordersInnerH))
            {
                for (size_t i = 0; i < InContext.Lights.size(); ++i)
                {
                    ImGui::TableNextColumn();

                    const bool bSelected = (InContext.SelectedLight == i);

                    std::string Label = "Light " + std::to_string(i);

                    if (ImGui::Selectable(Label.c_str(), bSelected, 0, ImVec2(80.0f, 0.0f)))
                    {
                        InContext.SelectedLight = i;
                    }
                }

                ImGui::EndTable();
            }

            ImGui::EndChild();

            // Properties of the Selected item
            Light& Selectable = InContext.Lights[InContext.SelectedLight];
            ImGui::ColorEdit3("Light Color", &Selectable.Color.x);

            ImGui::Checkbox("Animate Light", &Selectable.bAnimateLight);
            if (Selectable.bAnimateLight)
            {
                ImGui::BeginDisabled();
            }

            ImGui::DragFloat3("Light Position", &Selectable.Position.x, 0.1f);

            if (Selectable.bAnimateLight)
            {
                ImGui::EndDisabled();
            }
        }

        ImGui::SliderFloat("Light Ambient", &InContext.Settings.AmbientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Light Diffuse", &InContext.Settings.DiffuseStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Light Specular", &InContext.Settings.SpecularStrength, 0.0f, 1.0f);

        if (ImGui::Button("Add Light"))
        {
            GLfloat OffsetSpawnPos = static_cast<GLfloat>(InContext.Lights.size());

            InContext.Lights.push_back({glm::vec3(OffsetSpawnPos, 0.0f, 0.0f), glm::vec3(1)});
            InContext.SelectedLight = static_cast<int>(InContext.Lights.size()) - 1; // Select a new one
        }

        ImGui::SameLine();

        ImGui::BeginDisabled(InContext.Lights.size() <= 1);
        if (ImGui::Button("Remove Light"))
        {
            InContext.Lights.erase(InContext.Lights.begin() + InContext.SelectedLight);

            if (InContext.SelectedLight >= static_cast<int>(InContext.Lights.size()))
            {
                InContext.SelectedLight = static_cast<int>(InContext.Lights.size()) - 1;
            }
        }
        ImGui::EndDisabled();
    }

} // namespace

namespace FImGuiLayer
{
    void BuildUI(AppContext& InContext)
    {
        ImGui::Begin("Settings");

        ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

        ImGui::BeginChild("Settings Content", ImVec2(0, -25.0f));

        ImGui::Separator();
        ImGui::ColorEdit3("Background", InContext.Settings.BackgroundColor);

        ImGui::Separator();
        ImGui::Checkbox("Wireframe", &InContext.Settings.bWireframe);

        ImGui::Separator();
        CameraBuild(InContext.MainCamera);

        ImGui::Separator();
        LightsBuild(InContext);

        ImGui::EndChild();

        // Reset settings to default values
        if (ImGui::Button("Reset Defaults", ImVec2(-1.0f, 0.0f)))
        {
            InContext.Settings = DebugSettings{};
        }

        ImGui::End();
    }
} // namespace FImGuiLayer