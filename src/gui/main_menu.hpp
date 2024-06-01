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

    auto *font = ImGui::GetFont();
    font->Scale = 5.f;
    ImGui::PushFont(font);

    static constexpr auto *title = "Impostor Anomaly";

    auto window_width = ImGui::GetWindowSize().x;
    auto text_width = ImGui::CalcTextSize(title).x;

    ImGui::SetCursorPos({(window_width - text_width) * 0.5f, 100.f});
    ImGui::Text("Impostor Anomaly");

    ImGui::PopFont();

    font->Scale = 1.f;
    ImGui::PushFont(font);

    ImGui::SetCursorPos({io.DisplaySize.x / 2 - 100, io.DisplaySize.y / 2 - 50});
    if (ImGui::Button("Play Game", {200, 100})) {
        ret = true;
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    return ret;
}
