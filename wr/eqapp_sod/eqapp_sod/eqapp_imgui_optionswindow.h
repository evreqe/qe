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
            ImGui::InputInt("X", &g_hudX, 1, 10);
            ImGui::InputInt("Y", &g_hudY, 1, 10);
            ImGui::InputFloat("Width", &g_hudWidth, 1.0f, 10.0f);
            ImGui::InputFloat("Height", &g_hudHeight, 1.0f, 10.0f);
        }

        if (ImGui::CollapsingHeader("ESP"))
        {
            ImGui::Checkbox("Enabled",          &g_espIsEnabled);
            ImGui::Separator();

            ImGui::Columns(2, NULL, false);

                ImGui::Checkbox("Spawns",           &g_espSpawnIsEnabled);
                ImGui::Checkbox("Ground Spawns",    &g_espGroundSpawnIsEnabled);
                ImGui::Checkbox("Doors",            &g_espDoorIsEnabled);
                ImGui::Checkbox("Skeletons",        &g_espSkeletonIsEnabled);
                ImGui::Checkbox("Waypoints",        &g_espWaypointIsEnabled);
                ImGui::Checkbox("Custom",           &g_espCustomIsEnabled);

                ImGui::NextColumn();

                ImGui::InputFloat("",               &g_espSpawnDistance, 1.0f, 100.0f);
                ImGui::InputFloat("",               &g_espGroundSpawnDistance, 1.0f, 100.0f);
                ImGui::InputFloat("",               &g_espDoorDistance, 1.0f, 100.0f);
                ImGui::InputFloat("",               &g_espSkeletonDistance, 1.0f, 100.0f);
                ImGui::InputFloat("",               &g_espWaypointDistance, 1.0f, 100.0f);
                ImGui::InputFloat("",               &g_espCustomDistance, 1.0f, 100.0f);

            ImGui::Columns(1);
        }

        ImGui::End();
    }
};

#endif // EQAPP_IMGUI_OPTIONSWINDOW_H
