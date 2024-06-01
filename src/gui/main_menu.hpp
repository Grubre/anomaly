#pragma once

#include <fmt/core.h>
#include <imgui.h>

inline bool main_menu() {
    auto io = ImGui::GetIO();
    bool ret = false;
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 255));
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowFontScale(2.0f);

    ImGui::SetCursorPos({io.DisplaySize.x / 2 - 100, io.DisplaySize.y / 2 - 50});
    if (ImGui::Button("Play Game", {200, 100})) {
        ret=  true;
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    return ret;
}
