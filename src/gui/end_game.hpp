#pragma once
#include "assets/asset_manager.hpp"
#include "imgui.h"
#include "components/sprite.hpp"
#include "inspector.hpp"
#include "components/corpse.hpp"
namespace an{
inline bool end_scr(entt::registry &registry, WinCondition win) {
    static entt::entity last_character;
    auto asset_manager = registry.ctx().get<an::AssetManager>();
    auto background = asset_manager.get_texture(an::TextureEnum::UI_BACKGROUND).texture;
    auto gm = asset_manager.get_texture(an::TextureEnum::GAME_OVER).texture;
    auto win_tex = asset_manager.get_texture(an::TextureEnum::WIN).texture;
    auto io = ImGui::GetIO();
    bool ret = false;
    float x = io.DisplaySize.x * 0.8f;
    float y = io.DisplaySize.y * 0.8f;
    ImGui::SetNextWindowSize({x, y});
    ImGui::SetNextWindowPos({(io.DisplaySize.x - x) / 2, (io.DisplaySize.y - y) / 2});

    ImGui::Begin("Dialog", nullptr,ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
    ImGui::SetCursorPosX(x/3.3f);
    if(win == WinCondition::LOSE)
    {
        ImGui::Image((void *)&gm, {x/2, y/2});
    }
    else
    {
        ImGui::Image((void *)&win_tex, {x/2, y/2});
    }


    ImGui::SetCursorPosX(x/2);
    if (ImGui::Button("Restart", {200, 100})) {
        ret=  true;
    }

    ImGui::End();
    return ret;
}
}
