#pragma once

struct DebugSettings;
class Camera;

namespace ImGuiLayer
{
    void BuildUI(DebugSettings& InSettings, Camera& InCamera);

} // namespace ImGuiLayer