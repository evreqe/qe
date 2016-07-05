#ifndef EQAPP_IMGUI_OPTIONSWINDOW_H
#define EQAPP_IMGUI_OPTIONSWINDOW_H

#include "eqapp_imgui.h"

struct EQAPPIMGUIOptionsWindow
{
    EQAPPIMGUIOptionsWindow()
    {
        //
    }

    ~EQAPPIMGUIOptionsWindow()
    {
        //
    }

    void Draw(const char* title, bool* p_open)
    {
        ImGui::Begin(title, p_open);

        ImGui::Checkbox("Debug", &g_debugIsEnabled);

        if (ImGui::Button("Free Camera"))
        {
            EQAPP_FreeCamera_Toggle();
        }
        ImGui::SameLine();
        EQAPP_IMGUI_TextBool(g_freeCameraIsEnabled);

        if (ImGui::Button("Census"))
        {
            EQAPP_Census_Toggle();
        }
        ImGui::SameLine();
        EQAPP_IMGUI_TextBool(g_censusIsEnabled);

        if (ImGui::CollapsingHeader("HUD"))
        {
            ImGui::Checkbox("Enabled", &g_hudIsEnabled);
            ImGui::Separator();
            ImGui::SliderInt("X", &g_hudX, 0, g_videoModeWidth);
            ImGui::SliderInt("Y", &g_hudY, 0, g_videoModeHeight);
            ImGui::SliderFloat("Width", &g_hudWidth, 0, (float)g_videoModeWidth);
            ImGui::SliderFloat("Height", &g_hudHeight, 0, (float)g_videoModeHeight);
        }

        if (ImGui::CollapsingHeader("ESP"))
        {
            ImGui::Checkbox("Enabled",          &g_espIsEnabled);
            ImGui::Separator();
            ImGui::Checkbox("Spawns",           &g_espGroundSpawnIsEnabled);
            ImGui::SameLine();
            ImGui::Checkbox("Ground Spawns",    &g_espGroundSpawnIsEnabled);
            ImGui::Checkbox("Doors",            &g_espDoorIsEnabled);
            ImGui::SameLine();
            ImGui::Checkbox("Skeletons",        &g_espSkeletonIsEnabled);
            ImGui::Checkbox("Waypoints",        &g_espWaypointIsEnabled);
            ImGui::SameLine();
            ImGui::Checkbox("Custom",           &g_espCustomIsEnabled);
        }

        ImGui::End();
    }
};

#endif // EQAPP_IMGUI_OPTIONSWINDOW_H
