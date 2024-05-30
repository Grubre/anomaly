#include "assets/assets_loader.hpp"
#include "components/common.hpp"
#include "components/sprite.hpp"
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
#include "components/collisions.hpp"
#include "components/sprite.hpp"
#include "components/player.hpp"
#include "components/relations.hpp"

void load_resources(an::AssetManager &asset_manager) {
    using T = an::TextureEnum;
    using S = an::SoundEnum;

    auto player_img = an::load_asset(LoadImage, "player/player-test.png");
    auto test_tile = an::load_asset(LoadImage, "map/test-tile.png");
    asset_manager.register_texture(player_img, T::PLAYER_TEXTURE);
    asset_manager.register_texture(test_tile, T::TEST_TILE);
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
    auto &key_manager = registry.ctx().emplace<an::KeyManager>();
    auto &asset_manager = registry.ctx().emplace<an::AssetManager>();
    load_resources(asset_manager);
    auto inspector = an::Inspector<an::LocalTransform, an::GlobalTransform, an::Sprite, an::Alive, an::Health,
                                   an::Player, an::Velocity>(&registry);
    // player
    [[maybe_unused]] auto player = an::make_player(registry);
    // shader
    auto base_shader = an::load_asset(LoadShader, "shaders/base.vs", "shaders/base.fs");

    an::emplace<an::ShaderComponent>(registry, player, base_shader);
    // an::emplace<an::Sprite>(registry, entity);

    while (!WindowShouldClose()) {
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        an::notify_keyboard_press_system(key_manager);


        BeginDrawing();
        ClearBackground(RAYWHITE);
        // ======================================
        // DRAW SYSTEMS
        // ======================================
        an::destroy_unparented(registry);
        an::propagate_parent_transform(registry);


        an::render_sprites(registry);

        an::move_things(registry);

        an::static_vs_character_collision_system(registry);

        an::debug_draw_bodies(registry);

        // ======================================
        // DRAW GUI
        // ======================================
        rlImGuiBegin();

        ImGui::ShowDemoWindow();
        inspector.draw_gui();

        rlImGuiEnd();

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseAudioDevice();
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
