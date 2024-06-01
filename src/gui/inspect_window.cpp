#include "inspect_window.hpp"
#include "assets/asset_manager.hpp"
#include "components/sprite.hpp"
namespace an{

void draw_inspect_dialog(entt::registry &registry,entt::entity player ,entt::entity npc) {
    static entt::entity last_character;
    static TextureAsset character_texture; 

    auto io = ImGui::GetIO();
    auto asset_manager = registry.ctx().get<an::AssetManager>();
    auto sus_btn = asset_manager.get_texture(an::TextureEnum::B_SUS).texture;
    auto back_btn = asset_manager.get_texture(an::TextureEnum::B_BACK).texture;
    auto legit_btn = asset_manager.get_texture(an::TextureEnum::B_LEGIT).texture;
    auto background = asset_manager.get_texture(an::TextureEnum::UI_BACKGROUND).texture;

    if (npc != last_character) {
        last_character = npc;
        auto char_drawable = registry.get<Drawable>(npc);
        character_texture.unload();
        character_texture = std::get<CharacterSprite>(char_drawable.sprite).bake_to_texture(registry);
    }
    auto character = character_texture;

    // setup
    float x = io.DisplaySize.x * 0.8f;
    float y = io.DisplaySize.y * 0.8f;
    ImGui::SetNextWindowSize({x, y});
    ImGui::SetNextWindowPos({(io.DisplaySize.x - x) / 2, (io.DisplaySize.y - y) / 2});

    ImGui::Begin("Dialog", nullptr,ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
    ImGui::Image((void *)&background, {x, y});
    ImGui::Text("Hello, world!");
    ImGui::SetCursorPosY((y - 72 * 6) / 3);
    ImGui::SetCursorPosX((x - 64 * 6) / 2);
    ImGui::Image((void *)&character.texture, {64 * 6, 72 * 6});

    ImGui::SetCursorPosX((x - 256 * 3) / 2);
    //buttons without border
    if (ImGui::ImageButton((void *)&sus_btn, {128*2, 64*2})) {
        mark_entity(registry, npc);
        registry.remove<ShowUI>(player);
    }
    ImGui::SameLine();
    if (ImGui::ImageButton((void *)&legit_btn, {128*2, 64*2})) {
        registry.remove<Marked>(npc);
        registry.remove<ShowUI>(player);
    }
    ImGui::SameLine();
    if (ImGui::ImageButton((void *)&back_btn, {128*2, 64*2})) {
        registry.remove<ShowUI>(player);
        remove_interrupt(registry,npc);
    }
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
        draw_inspect_dialog(registry, entity,npc);
        co++;
    }
}
}

