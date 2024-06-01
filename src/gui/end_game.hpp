#pragma once
#include "assets/asset_manager.hpp"
#include "imgui.h"
#include "components/sprite.hpp"
#include "inspector.hpp"
namespace an{
inline bool main_menu(entt::registry &registry) {
    static entt::entity last_character;
    auto asset_manager = registry.ctx().get<an::AssetManager>();
    auto background = asset_manager.get_texture(an::TextureEnum::UI_BACKGROUND).texture;
    auto io = ImGui::GetIO();
    bool ret = false;
    float x = io.DisplaySize.x * 0.8f;
    float y = io.DisplaySize.y * 0.8f;
    ImGui::SetNextWindowSize({x, y});
    ImGui::SetNextWindowPos({(io.DisplaySize.x - x) / 2, (io.DisplaySize.y - y) / 2});

    ImGui::Begin("Dialog", nullptr,ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
    ImGui::Image((void *)&background, {x, y});
    ImGui::Text("Hello, world!");

    ImGui::SetCursorPos({io.DisplaySize.x / 2 - 100, io.DisplaySize.y / 2 - 50});
    if (ImGui::Button("Play Game", {200, 100})) {
        ret=  true;
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    return ret;
}
}
