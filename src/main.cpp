#include "assets/assets_loader.hpp"
#include "components/collisions.hpp"
#include "components/common.hpp"
#include "components/sprite.hpp"
#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>
#include <fmt/printf.h>
#include <entt.hpp>
#include "keyinput.hpp"
#include "assets/asset_manager.hpp"
#include "gui/inspector.hpp"
#include "components/velocity.hpp"
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
void default_keys(an::KeyManager &key_manager) {
    key_manager.assign_key(KEY_W, an::KeyEnum::MOVE_UP);
    key_manager.assign_key(KEY_S, an::KeyEnum::MOVE_DOWN);
    key_manager.assign_key(KEY_A, an::KeyEnum::MOVE_LEFT);
    key_manager.assign_key(KEY_D, an::KeyEnum::MOVE_RIGHT);
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
    default_keys(key_manager);
    auto &asset_manager = registry.ctx().emplace<an::AssetManager>();
    load_resources(asset_manager);
    auto inspector = an::Inspector<an::LocalTransform, an::GlobalTransform, an::Sprite, an::Alive, an::Health,
                                   an::Player, an::Velocity, an::CharacterBody, an::StaticBody>(&registry);

    // camera
    registry.ctx().emplace<Camera2D>(Vector2(GetScreenWidth()/2, GetScreenHeight()/2), Vector2(), 0.f, 2.f);

    auto entity = registry.create();
    an::emplace<an::Sprite>(registry, entity, an::TextureEnum::TEST_TILE);
    // player
    [[maybe_unused]] auto player = an::make_player(registry);
    // shader
    auto base_shader = an::load_asset(LoadShader, "shaders/base.vs", "shaders/base.fs");

    an::emplace<an::ShaderComponent>(registry, player, base_shader);
    // an::emplace<an::Sprite>(registry, entity);

    // test collider
    auto test_collider = registry.create();
    an::emplace<an::GlobalTransform>(registry, test_collider);
    an::emplace<an::StaticBody>(registry, test_collider, Vector2(100.f,100.f), Vector2(500.f, 300.f));

    // test char collider
    auto test_char_collider = registry.create();
    an::emplace<an::GlobalTransform>(registry, test_char_collider);
    an::emplace<an::CharacterBody>(registry, test_char_collider, Vector2(800.f,800.f), 50.f);

    while (!WindowShouldClose()) {
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        an::notify_keyboard_press_system(key_manager);
        an::destroy_unparented(registry);
        an::propagate_parent_transform(registry);
        an::update_player(registry, player);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        // ======================================
        // DRAW SYSTEMS
        // ======================================

        an::move_things(registry);

        auto pos = registry.get<an::GlobalTransform>(player);
        registry.ctx().get<Camera2D>().target = pos.transform.position;

        an::static_vs_character_collision_system(registry);
        an::character_vs_character_collision_system(registry);

        BeginMode2D(registry.ctx().get<Camera2D>());

        an::render_sprites(registry);
        an::debug_draw_bodies(registry);

        EndMode2D();
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
