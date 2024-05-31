#pragma once
#include <imgui.h>
#include <entt.hpp>
#include "assets/asset_manager.hpp"
namespace an {
void draw_inspect_dialog(entt::registry &registry){
    auto io = ImGui::GetIO();
    auto asset_manager = registry.ctx().get<an::AssetManager>();
    auto sus_btn =asset_manager.get_texture(an::TextureEnum::B_SUS).texture;
    auto back_btn =asset_manager.get_texture(an::TextureEnum::B_BACK).texture;
    auto legit_btn =asset_manager.get_texture(an::TextureEnum::B_LEGIT).texture;
    auto character =asset_manager.get_texture(an::TextureEnum::BASE_CHARACTER).texture;
    //setup
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::SetNextWindowPos({0, 0});


    ImGui::Begin("Dialog");
    ImGui::Text("Hello, world!");

    ImGui::Image((void *)&character, {64*4, 72*4});
    ImGui::ImageButton((void *)&sus_btn,{300,200});
    ImGui::ImageButton((void *)&legit_btn,{300,200});
    ImGui::ImageButton((void *)&back_btn,{300,200});
    ImGui::End();
}

}