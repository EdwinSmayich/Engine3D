#include "ImGuiLayer.h"

#include <imgui.h>
#include "../Core/AppContext.h"

namespace FUIFunction
{
    void TransformObjectOnScene(FAppContext& InContext, FSceneObject& InObj)
    {
        ImGui::Text("Gizmo:");
        if (ImGui::RadioButton("Move", InContext.GizmoOperation == EGizmoOperation::EGO_Translate))
        {
            InContext.GizmoOperation = EGizmoOperation::EGO_Translate;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", InContext.GizmoOperation == EGizmoOperation::EGO_Rotate))
        {
            InContext.GizmoOperation = EGizmoOperation::EGO_Rotate;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", InContext.GizmoOperation == EGizmoOperation::EGO_Scale))
        {
            InContext.GizmoOperation = EGizmoOperation::EGO_Scale;
        }
        if (ImGui::RadioButton("World", InContext.GizmoMode == EGizmoMode::EGM_World))
        {
            InContext.GizmoMode = EGizmoMode::EGM_World;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Local", InContext.GizmoMode == EGizmoMode::EGM_Local))
        {
            InContext.GizmoMode = EGizmoMode::EGM_Local;
        }

        ImGui::Separator();
        ImGui::DragFloat3("Position", &InObj.Transform.Position.x, 0.1f);

        ImGui::Text("Rotate (delta):");
        ImGui::Text("Quat: %.3f %.3f %.3f %.3f", InObj.Transform.Rotation.x, InObj.Transform.Rotation.y, InObj.Transform.Rotation.z,
                    InObj.Transform.Rotation.w);

        if (ImGui::Button("X +15"))
        {
            InObj.Transform.Rotation = InObj.Transform.Rotation * glm::angleAxis(glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        ImGui::SameLine();
        if (ImGui::Button("Y +15"))
        {
            InObj.Transform.Rotation = InObj.Transform.Rotation * glm::angleAxis(glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        ImGui::SameLine();
        if (ImGui::Button("Z +15"))
        {
            InObj.Transform.Rotation = InObj.Transform.Rotation * glm::angleAxis(glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (ImGui::DragFloat3("RotationEuler", &InObj.Transform.RotationEuler.x, 1.0f))
        {
            InObj.Transform.UpdateRotationFromEuler();
        }

        ImGui::DragFloat3("Scale", &InObj.Transform.Scale.x, 0.05f, 0.05f, 20.0f);
    }
} // namespace FUIFunction

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
            ImGui::NewLine();
        }
    }

    void SceneBuild(FAppContext& InContext)
    {
        if (ImGui::CollapsingHeader("Scene Objects", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BeginChild("Objects", ImVec2(0.0f, 100.0f), ImGuiChildFlags_Borders);

            if (ImGui::BeginTable("SortTable", 3, ImGuiTableFlags_BordersInnerH))
            {
                for (size_t i = 0; i < InContext.SceneObjects.size(); ++i)
                {
                    ImGui::TableNextColumn();

                    const bool bSelected = (InContext.SelectedObject == i);

                    const bool bIsLight = (InContext.SceneObjects[i].ObjectType == EObjectType::EOT_Light);
                    std::string Label = (bIsLight ? "Light_" : "Cube_") + std::to_string(i);

                    if (ImGui::Selectable(Label.c_str(), bSelected, 0, ImVec2(80.0f, 0.0f)))
                    {
                        InContext.SelectedObject = i;
                    }
                }

                ImGui::EndTable();
            }

            ImGui::EndChild();

            // Properties of the Selected item
            FSceneObject& Selectable = InContext.SceneObjects[InContext.SelectedObject];

            FUIFunction::TransformObjectOnScene(InContext, Selectable);

            // Ambient settings
            ImGui::Separator();
            ImGui::SliderFloat("Ambient Strength", &InContext.Settings.AmbientStrength, 0.0f, 1.0f);

            if (Selectable.ObjectType == EObjectType::EOT_Light)
            {
                ImGui::ColorEdit3("Light Color", &Selectable.LightData.Color.x);
                ImGui::Checkbox("Animate Light", &Selectable.LightData.bAnimateLight);

                // Type of Lighting
                const char* LightTypeNames[] = {"Direction", "Point", "Spot"};

                int CurrentType = static_cast<int>(Selectable.LightData.LightingType);
                if (ImGui::Combo("Type", &CurrentType, LightTypeNames, IM_ARRAYSIZE(LightTypeNames)))
                {
                    Selectable.LightData.LightingType = static_cast<LightType>(CurrentType);
                }

                switch (Selectable.LightData.LightingType)
                {
                    case LightType::ELT_Directional:
                    {
                        ImGui::DragFloat3("Direction", &Selectable.LightData.Direction.x, 0.001f, -1.0f, 1.0f);
                        break;
                    }
                    case LightType::ELT_Point:
                    {
                        ImGui::DragFloat("Linear", &Selectable.LightData.Linear, 0.001f, 0.0f, 1.0f);
                        ImGui::DragFloat("Quadratic", &Selectable.LightData.Quadratic, 0.0001f, 0.0f, 1.0f);
                        break;
                    }
                    case LightType::ELT_Spot:
                    {
                        GLfloat InnerAngle = glm::degrees(glm::acos(Selectable.LightData.InnerCutoff));
                        GLfloat OuterAngle = glm::degrees(glm::acos(Selectable.LightData.OuterCutoff));

                        ImGui::DragFloat3("Direction", &Selectable.LightData.Direction.x, 0.01f, -1.0f, 1.0f);
                        if (ImGui::SliderFloat("Inner Cutoff", &InnerAngle, 0.0f, 90.0f))
                        {
                            Selectable.LightData.InnerCutoff = glm::cos(glm::radians(glm::min(InnerAngle, OuterAngle)));
                        }
                        if (ImGui::SliderFloat("Outer Cutoff", &OuterAngle, 0.0f, 90.0f))
                        {
                            Selectable.LightData.OuterCutoff = glm::cos(glm::radians(glm::max(OuterAngle, InnerAngle)));
                        }

                        break;
                    }
                }
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Add Light"))
        {
            glm::vec3 SpawnPos = InContext.MainCamera.GetPosition() + InContext.MainCamera.GetFrontVector() * 10.0f;
            InContext.SceneObjects.push_back({EObjectType::EOT_Light, {SpawnPos}, 0.4f, {}});
            InContext.SelectedObject = static_cast<int>(InContext.SceneObjects.size()) - 1; // Select a new one
        }

        ImGui::SameLine();
        if (ImGui::Button("Add Cube"))
        {
            glm::vec3 SpawnPos = InContext.MainCamera.GetPosition() + InContext.MainCamera.GetFrontVector() * 10.0f;
            InContext.SceneObjects.push_back({EObjectType::EOT_Cube, {SpawnPos}, 1.0f, {}});
            InContext.SelectedObject = static_cast<int>(InContext.SceneObjects.size()) - 1; // Select a new one
        }

        ImGui::SameLine();
        ImGui::BeginDisabled(InContext.SceneObjects.size() <= 1);
        if (ImGui::Button("Remove Object"))
        {
            InContext.SceneObjects.erase(InContext.SceneObjects.begin() + InContext.SelectedObject);

            if (InContext.SelectedObject >= static_cast<int>(InContext.SceneObjects.size()))
            {
                InContext.SelectedObject = static_cast<int>(InContext.SceneObjects.size()) - 1;
            }
        }
        ImGui::EndDisabled();
    }
} // namespace

namespace FImGuiLayer
{
    void BuildUI(FAppContext& InContext)
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
        SceneBuild(InContext);

        ImGui::EndChild();

        // Reset settings to default values
        if (ImGui::Button("Reset Defaults", ImVec2(-1.0f, 0.0f)))
        {
            InContext.ResetAppContextToDefaults();
        }

        ImGui::End();
    }
} // namespace FImGuiLayer