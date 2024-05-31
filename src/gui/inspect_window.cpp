#include "inspect_window.hpp"
namespace an{
void draw_inspect_dialog(entt::registry &registry, entt::entity entity) {
    auto io = ImGui::GetIO();
    auto asset_manager = registry.ctx().get<an::AssetManager>();
    auto sus_btn = asset_manager.get_texture(an::TextureEnum::B_SUS).texture;
    auto back_btn = asset_manager.get_texture(an::TextureEnum::B_BACK).texture;
    auto legit_btn = asset_manager.get_texture(an::TextureEnum::B_LEGIT).texture;
    auto character = asset_manager.get_texture(an::TextureEnum::BASE_CHARACTER).texture;
    // setup
    float x = io.DisplaySize.x * 0.8f;
    float y = io.DisplaySize.y * 0.8f;
    ImGui::SetNextWindowSize({x, y});
    ImGui::SetNextWindowPos({(io.DisplaySize.x - x) / 2, (io.DisplaySize.y - y) / 2});

    ImGui::Begin("Dialog");
    ImGui::Text("Hello, world!");

    ImGui::Image((void *)&character, {64 * 4, 72 * 4});
    ImGui::ImageButton((void *)&sus_btn, {300, 200});
    ImGui::SameLine();
    ImGui::ImageButton((void *)&legit_btn, {300, 200});
    ImGui::SameLine();
    ImGui::ImageButton((void *)&back_btn, {300, 200});
    ImGui::End();
}
void update_ui(entt::registry &registry, entt::entity entity) {
    auto view = registry.view<Interrupted>();
    auto co=0;
    for (auto &&[npc] : view.each()) {
        if(co>0){
            remove_interrupt(registry,npc);
            continue;
        }
        if(!registry.all_of<ShowUI>(entity)){
            remove_interrupt(registry,npc);
            continue;
        }
        draw_inspect_dialog(registry, npc);
        co++;
    }
}
}

