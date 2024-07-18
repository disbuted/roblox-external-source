#include "esp.hpp"

#include "../../utils/overlay/imgui/imgui.h"
#include "../globals/globals.hpp"

#include <Windows.h>
#include <iomanip>


float calc_dist_3d(silence::roblox::vector3_t first, silence::roblox::vector3_t sec)
{
    return sqrt((first.x - sec.x) * (first.x - sec.x) + (first.y - sec.y) * (first.y - sec.y) + (first.z - sec.z) * (first.z - sec.z));
}

silence::roblox::vector3_t vector_sub(silence::roblox::vector3_t one, silence::roblox::vector3_t two)
{
    return { one.x - two.x, one.y - two.y, one.z - two.z };
}

float vector3_mag(silence::roblox::vector3_t vector)
{
    return sqrtf((vector.x * vector.y) + (vector.y * vector.y) + (vector.z * vector.z));
}

void DrawEsp(int X, int Y, int W, int H, ImColor Color) {
    ImVec4 color = ImVec4(globals::box_color[0], globals::box_color[1], globals::box_color[2], 1.0f);
    ImU32 outlineColor = IM_COL32(0, 0, 0, 255);
    float lineT = 1.0f;
    float outlineThickness = 2.0f;

    float lineW = (W / 5);
    float lineH = (H / 6);

    auto draw = ImGui::GetBackgroundDrawList();


    if (globals::outline) {
        draw->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), outlineColor, lineT + outlineThickness);

        draw->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), outlineColor, lineT + outlineThickness);

        draw->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), outlineColor, lineT + outlineThickness);

        draw->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), outlineColor, lineT + outlineThickness);

        draw->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), outlineColor, lineT + outlineThickness);

        draw->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), outlineColor, lineT + outlineThickness);

        draw->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), outlineColor, lineT + outlineThickness);

        draw->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), outlineColor, lineT + outlineThickness);


    }

    draw->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::ColorConvertFloat4ToU32(color), lineT);

    draw->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::ColorConvertFloat4ToU32(color), lineT);

    draw->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::ColorConvertFloat4ToU32(color), lineT);

    draw->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::ColorConvertFloat4ToU32(color), lineT);

    draw->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::ColorConvertFloat4ToU32(color), lineT);

    draw->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::ColorConvertFloat4ToU32(color), lineT);

    draw->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(color), lineT);

    draw->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(color), lineT);
}


void silence::roblox::hook_esp()
{
    auto players = globals::players;
    auto draw = ImGui::GetBackgroundDrawList();

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    ImVec2 center = ImVec2(screenWidth / 2, screenHeight / 2);

    POINT cursor_pos;
    GetCursorPos(&cursor_pos);
    ScreenToClient(FindWindowA(0, "Roblox"), &cursor_pos);

    silence::roblox::vector3_t camerapos = globals::visualengine.get_camera_pos();

    if (globals::fov_on)
    {
        ImColor black_outline_color(0, 0, 0);

        ImVec4 color = ImVec4(globals::fov_color[0], globals::fov_color[1], globals::fov_color[2], 1.0f);

        ImVec4 fovfill_color = ImVec4(globals::fov_fill_color[0], globals::fov_fill_color[1], globals::fov_fill_color[2], 0.4f);

        draw->AddCircle(center, globals::fov + 1.0f, black_outline_color, 64, 1.0f);
        draw->AddCircle(center, globals::fov, ImGui::ColorConvertFloat4ToU32(color), 64, 1.0f);
        draw->AddCircle(center, globals::fov - 1.0f, black_outline_color, 64, 1.0f);
        if (globals::fov_filled) {
            draw->AddCircleFilled(center, globals::fov - 1.5f, ImGui::ColorConvertFloat4ToU32(fovfill_color), 64);
        }
    }

    if (globals::show_deadzone)
    {
        ImColor fill_color(0, 0, 0, 51);
        ImColor black_outline_color(0, 0, 0);

        ImVec4 color = ImVec4(globals::deadzone_color[0], globals::deadzone_color[1], globals::deadzone_color[2], 1.0f);

        ImVec4 deadzone_fill_color = ImVec4(globals::fov_fill_color[0], globals::fov_fill_color[1], globals::fov_fill_color[2], 0.4f);

        draw->AddCircle(ImVec2(cursor_pos.x, cursor_pos.y), globals::deadzone_value + 1.0f, black_outline_color, 64, 1.0f);
        draw->AddCircle(ImVec2(cursor_pos.x, cursor_pos.y), globals::deadzone_value, ImGui::ColorConvertFloat4ToU32(color), 64, 1.0f);
        draw->AddCircle(ImVec2(cursor_pos.x, cursor_pos.y), globals::deadzone_value - 1.0f, black_outline_color, 64, 1.0f);
        if (globals::deadzone_fov_filled) {
            draw->AddCircleFilled(ImVec2(cursor_pos.x, cursor_pos.y), globals::fov - 1.5f, ImGui::ColorConvertFloat4ToU32(deadzone_fill_color), 64);
        }
    }

    if (globals::esp)
    {
        auto dimensions = globals::visualengine.get_dimensions();
        auto viewmatrix = globals::visualengine.get_view_matrix();
        auto localplayer = players.get_local_player();
        auto localplayerHead = localplayer.get_model_instance().find_first_child("Head");

        for (auto player : players.children())
        {

            if (player.self == localplayer.self && !globals::local_box)
                continue;

            // localplayer check
            if (player.self == localplayer.self)
                continue;

            auto character = player.get_model_instance();
            if (!character.self)
                continue;

            auto humanoid = character.find_first_child("Humanoid");
            if (!humanoid.self)
                continue;

            auto head = character.find_first_child("Head");
            if (!head.self)
                continue;

            if (humanoid.get_health() <= 4 && globals::knock_check)
            {
                continue;
            }

            auto head_position_3d = head.get_part_pos();

            auto head_position = silence::roblox::world_to_screen(vector_sub(head_position_3d, { 0, 2, 0 }), dimensions, viewmatrix);
            auto leg_pos = silence::roblox::world_to_screen(vector_sub(head_position_3d, { 0, 5, 0 }), dimensions, viewmatrix);

            auto hrp_pos = humanoid.get_part_pos();
            auto headPosY = static_cast<float>(hrp_pos.y + 2.5);
            auto legPosY = static_cast<float>(hrp_pos.y - 3.5);
            auto top = silence::roblox::world_to_screen({ static_cast<float>(hrp_pos.x), headPosY, static_cast<float>(hrp_pos.z) }, dimensions, viewmatrix);
            auto bottom = silence::roblox::world_to_screen({ static_cast<float>(hrp_pos.x), legPosY, static_cast<float>(hrp_pos.z) }, dimensions, viewmatrix);

            if (leg_pos.x == -1)
                continue;

            float distance_from_camera = vector3_mag(vector_sub(camerapos, head_position_3d));

            float height = head_position.y - leg_pos.y;
            float width = height / 1.6f;

            // tracers -> line
            if (globals::tracers)
            {
                ImVec4 color = ImVec4(globals::tracers_color[0], globals::tracers_color[1], globals::tracers_color[2], 1.0f);
                draw->AddLine(ImVec2(head_position.x, head_position.y), ImVec2(static_cast<float>(cursor_pos.x), static_cast<float>(cursor_pos.y)), ImGui::ColorConvertFloat4ToU32(color));
            }


            // name esp -> text
            if (globals::name_esp)
            {

                std::string playerName = player.name();

                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2 - 1, top.y - 20 - 1 }, ImColor(0, 0, 0, 255), playerName.c_str());
                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2 - 1, top.y - 20 }, ImColor(0, 0, 0, 255), playerName.c_str());
                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2 - 1, top.y - 20 + 1 }, ImColor(0, 0, 0, 255), playerName.c_str());
                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2, top.y - 20 - 1 }, ImColor(0, 0, 0, 255), playerName.c_str());
                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2, top.y - 20 + 1 }, ImColor(0, 0, 0, 255), playerName.c_str());
                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2 + 1, top.y - 20 - 1 }, ImColor(0, 0, 0, 255), playerName.c_str());
                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2 + 1, top.y - 20 }, ImColor(0, 0, 0, 255), playerName.c_str());
                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2 + 1, top.y - 20 + 1 }, ImColor(0, 0, 0, 255), playerName.c_str());

                ImVec4 color(globals::name_color[0], globals::name_color[1], globals::name_color[2], 1.0f);
                draw->AddText({ (top.x + bottom.x) / 2 - ImGui::CalcTextSize(playerName.c_str()).x / 2, top.y - 20 }, ImGui::ColorConvertFloat4ToU32(color), playerName.c_str());
            }

            if (globals::distance_esp) {
                auto distance_position = silence::roblox::world_to_screen(vector_sub(head_position_3d, { -0.25, 5.3, 0 }), dimensions, viewmatrix);

                float distanceMagnitude = vector3_mag(vector_sub(localplayerHead.get_part_pos(), head_position_3d));
                distanceMagnitude = roundf(distanceMagnitude * 100) / 100;

                std::stringstream stream;
                stream << std::fixed << std::setprecision(0) << distanceMagnitude;
                std::string distanceStr = stream.str() + "m";

                float textWidth = ImGui::CalcTextSize(distanceStr.c_str()).x;
                ImVec2 distancePos = ImVec2(distance_position.x - (textWidth / 2), distance_position.y);

                draw->AddText(ImVec2(distancePos.x, distancePos.y) + ImVec2(-1, -1), ImColor(0, 0, 0), distanceStr.c_str());
                draw->AddText(ImVec2(distancePos.x, distancePos.y) + ImVec2(1, -1), ImColor(0, 0, 0), distanceStr.c_str());
                draw->AddText(ImVec2(distancePos.x, distancePos.y) + ImVec2(-1, 1), ImColor(0, 0, 0), distanceStr.c_str());
                draw->AddText(ImVec2(distancePos.x, distancePos.y) + ImVec2(1, 1), ImColor(0, 0, 0), distanceStr.c_str());
                draw->AddText(ImVec2(distancePos.x, distancePos.y), ImColor(255, 255, 255), distanceStr.c_str());
            }

            if (globals::healthbar)
            {
                auto humanoid = character.find_first_child("Humanoid");

                float health = humanoid.get_health();
                int healthPercentage = static_cast<int>((health / humanoid.get_max_health()) * 100);

                ImVec2 barPos = ImVec2(leg_pos.x - (width / 0.75f), leg_pos.y);
                ImVec2 barSize = ImVec2(3.5, height * 2); // Ensure this height correctly represents from the top of the bar to the feet of the character

                draw->AddRectFilled(barPos, ImVec2(barPos.x + barSize.x, barPos.y + barSize.y), ImColor(0, 0, 0, 150));

                // Calculate healthBarHeight based on the health percentage and the total height of the bar
                float healthBarHeight = (static_cast<float>(healthPercentage) / 100) * (barSize.y - 2);

                // Ensure the health bar's height does not exceed the intended height
                if (healthBarHeight > barSize.y - 2) {
                    healthBarHeight = barSize.y - 2;
                }

                // Apply the healthBarHeight to the position of the health bar
                draw->AddRectFilled(barPos + ImVec2(1, 1), ImVec2(barPos.x + barSize.x - 1, barPos.y + healthBarHeight), ImColor(0, 255, 0, 255));
            }

            //health info -> text
            if (globals::healthinfo)
            {
                auto humanoid = character.find_first_child("Humanoid");
                if (humanoid.self)
                {
                    float health = humanoid.get_health();
                    float maxHealth = humanoid.get_max_health();

                    // Check if maxHealth is not zero to avoid division by zero
                    if (maxHealth != 0) {
                        // int healthPercentage = static_cast<int>((health / maxHealth) * 100);
                        int healthPercentage = static_cast<int>(health);
                        float health = humanoid.get_health();

                        ImVec4 healthColor;

                        // Adjust the health color logic based on the health percentage
                        if (healthPercentage <= 100)
                            healthColor = ImColor(66, 245, 164);

                        if (healthPercentage <= 90)
                            healthColor = ImColor(66, 245, 111);

                        if (healthPercentage <= 70)
                            healthColor = ImColor(239, 245, 66);

                        if (healthPercentage <= 60)
                            healthColor = ImColor(245, 215, 66);

                        if (healthPercentage <= 50)
                            healthColor = ImColor(245, 176, 66);

                        if (healthPercentage <= 40)
                            healthColor = ImColor(245, 150, 66);

                        if (healthPercentage <= 30)
                            healthColor = ImColor(245, 123, 66);

                        if (healthPercentage <= 20)
                            healthColor = ImColor(245, 102, 66);

                        if (healthPercentage <= 10)
                            healthColor = ImColor(245, 66, 66);

                        if (healthPercentage <= 0)
                            healthColor = ImColor(245, 12, 12);


                        std::string text = std::to_string(healthPercentage) + "%";

                        // Draw the text with shadow for better readability
                        draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2) + ImVec2(-1, -1), ImColor(0, 0, 0), text.c_str());
                        draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2) + ImVec2(1, -1), ImColor(0, 0, 0), text.c_str());
                        draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2) + ImVec2(-1, 1), ImColor(0, 0, 0), text.c_str());
                        draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2) + ImVec2(1, 1), ImColor(0, 0, 0), text.c_str());

                        draw->AddText(ImVec2(head_position.x - width / 2, head_position.y + height + 2), (ImColor)healthColor, text.c_str());

                    }
                }
            }
            if (globals::box)
            {
                ImVec4 color = ImVec4(globals::box_color[0], globals::box_color[1], globals::box_color[2], 1.0f);

                float outline_offset = 0.1f;

                ImVec4 fill_color = ImVec4(globals::fill_color[0], globals::fill_color[1], globals::fill_color[2], 0.3f);
                ImColor black_outline_color(0, 0, 0);
                ImColor white_outline_color(255, 255, 255);

                switch (globals::boxtype)
                {
                case 0:
                    if (globals::filledbox) {
                        draw->AddRectFilled(ImVec2(leg_pos.x - (width / 1.3f) - outline_offset, leg_pos.y - outline_offset), ImVec2(head_position.x + width + outline_offset, head_position.y + height + outline_offset), ImGui::ColorConvertFloat4ToU32(fill_color));
                    }
                    draw->AddRect(ImVec2(leg_pos.x - (width / 1.3f), leg_pos.y), ImVec2(head_position.x + width, head_position.y + height), ImGui::ColorConvertFloat4ToU32(color));
                    if (globals::outline) {
                        draw->AddRect(ImVec2(leg_pos.x - (width / 1.3f) - outline_offset - 1.5, leg_pos.y - outline_offset - 1.5), ImVec2(head_position.x + width + outline_offset + 1.5, head_position.y + height + outline_offset + 1.5), black_outline_color);
                        draw->AddRect(ImVec2(leg_pos.x - (width / 1.3f) - outline_offset + 1.5, leg_pos.y - outline_offset + 1.5), ImVec2(head_position.x + width + outline_offset - 1.5, head_position.y + height + outline_offset - 1.5), black_outline_color);
                    }
                    if (globals::rainbowbox) {
                        float rainbow_width = (head_position.x + width + outline_offset) - (leg_pos.x - (width / 1.3f) - outline_offset);
                        float rainbow_height = (head_position.y + height + outline_offset) - (leg_pos.y - outline_offset);

                        ImU32 col_top_left = IM_COL32(231, 0, 0, 80);
                        ImU32 col_top_right = IM_COL32(255, 241, 0, 80);
                        ImU32 col_bottom_right = IM_COL32(0, 129, 31, 80);
                        ImU32 col_bottom_left = IM_COL32(0, 70, 173, 80);

                        draw->AddRectFilledMultiColor(
                            ImVec2(leg_pos.x - (width / 1.3f) - outline_offset, leg_pos.y - outline_offset),
                            ImVec2(leg_pos.x - (width / 1.3f) - outline_offset + rainbow_width, leg_pos.y - outline_offset + rainbow_height),
                            col_top_left,
                            col_top_right,
                            col_bottom_right,
                            col_bottom_left
                        );
                    }
                    break;
                case 1:
                    if (globals::filledbox) {
                        draw->AddRectFilled(ImVec2(leg_pos.x - (width / 1.3f) - outline_offset, leg_pos.y - outline_offset), ImVec2(head_position.x + width + outline_offset, head_position.y + height + outline_offset), ImGui::ColorConvertFloat4ToU32(fill_color));
                    }

                    if (globals::rainbowbox) {
                        float rainbow_width = (head_position.x + width + outline_offset) - (leg_pos.x - (width / 1.3f) - outline_offset);
                        float rainbow_height = (head_position.y + height + outline_offset) - (leg_pos.y - outline_offset);

                        ImU32 col_top_left = IM_COL32(231, 0, 0, 80);
                        ImU32 col_top_right = IM_COL32(255, 241, 0, 80);
                        ImU32 col_bottom_right = IM_COL32(0, 129, 31, 80);
                        ImU32 col_bottom_left = IM_COL32(0, 70, 173, 80);

                        draw->AddRectFilledMultiColor(
                            ImVec2(leg_pos.x - (width / 1.3f) - outline_offset, leg_pos.y - outline_offset),
                            ImVec2(leg_pos.x - (width / 1.3f) - outline_offset + rainbow_width, leg_pos.y - outline_offset + rainbow_height),
                            col_top_left,
                            col_top_right,
                            col_bottom_right,
                            col_bottom_left
                        );
                    }

                    DrawEsp(leg_pos.x - (width / 1.3f), leg_pos.y, head_position.x + width - leg_pos.x + (width / 1.3f), head_position.y + height - leg_pos.y, ImGui::ColorConvertFloat4ToU32(color));
                    break;
                }
            }
            if (globals::crosshair_on)
            {
                ImColor fill_color(0, 0, 0, 51);
                ImColor black_outline_color(0, 0, 0);

                ImVec4 color = ImVec4(globals::fov_color[0], globals::fov_color[1], globals::fov_color[2], 1.0f);

                ImVec4 fovfill_color = ImVec4(globals::fov_fill_color[0], globals::fov_fill_color[1], globals::fov_fill_color[2], 0.4f);

                // Adjust the size of the crosshair
                float fovRadiusX = 10.0f; // Width
                float fovRadiusY = 5.0f; // Height

                // Calculate center of the screen
                ImVec2 center = ImVec2(screenWidth / 2, screenHeight / 2);

                draw->AddLine(ImVec2(center.x - fovRadiusX, center.y), ImVec2(center.x + fovRadiusX, center.y), ImGui::ColorConvertFloat4ToU32(color), 1.0f);
                draw->AddLine(ImVec2(center.x, center.y - fovRadiusY), ImVec2(center.x, center.y + fovRadiusY), ImGui::ColorConvertFloat4ToU32(color), 1.0f);

                if (globals::fov_filled) {
                    draw->AddCircleFilled(center, fovRadiusX - 1.5f, ImGui::ColorConvertFloat4ToU32(fovfill_color), 64);
                }
            }

        }
    }
}
