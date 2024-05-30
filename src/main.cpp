#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>
#include <fmt/printf.h>
#include <entt.hpp>
#include "keyinput.hpp"
#include "assets/asset_manager.hpp"
#include "assets/assets_loader.hpp"
#include "gui/inspector.hpp"
#include "components/velocity.hpp"
#include "components/sprite.hpp"
#include "components/player.hpp"
#include "components/relations.hpp"

void load_resources(bh::AssetManager &asset_manager) {
    using T = bh::TextureEnum;
    using S = bh::SoundEnum;
    auto player_img = bh::load_asset(LoadImage, "player.png");
    asset_manager.register_texture(player_img, T::PLAYER_TEXTURE, 100, 200);
}

void setup_raylib() {
    const auto display = GetCurrentMonitor();
    const int screen_width = GetMonitorWidth(display);
    const int screen_height = GetMonitorHeight(display);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // SetConfigFlags(FLAG_VSYNC_HINT);

    fmt::println("Resolution is: {}x{}", screen_width, screen_height);
    InitWindow(screen_width, screen_height, "Hello World");
    ToggleFullscreen();
    InitAudioDevice();
}

auto main() -> int {
    // setup
    setup_raylib();
    rlImGuiSetup(true);
    auto registry = entt::registry();
    auto &key_manager = registry.ctx().emplace<bh::KeyManager>();
    auto &asset_manager = registry.ctx().emplace<bh::AssetManager>();
    load_resources(asset_manager);
    auto inspector = bh::Inspector<bh::LocalTransform, bh::GlobalTransform, bh::Sprite, bh::Alive, bh::Health,
                                   bh::Player, bh::Velocity>(&registry);
    // player
    [[maybe_unused]] auto player = bh::make_player(registry);
    // shader
    auto base_shader = bh::load_asset(LoadShader, "shaders/base.vs", "shaders/base.fs");
    while (!WindowShouldClose()) {
        bh::notify_keyboard_press_system(key_manager);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        bh::destroy_unparented(registry);
        bh::propagate_parent_transform(registry);

        bh::render_sprites(registry);

        bh::move_things(registry);

        rlImGuiBegin();

        ImGui::ShowDemoWindow();
        inspector.draw_gui();

        rlImGuiEnd();

        EndDrawing();
    }
    CloseAudioDevice();
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
