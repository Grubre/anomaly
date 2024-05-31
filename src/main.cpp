#include "assets/assets_loader.hpp"
#include "character_generator.hpp"
#include "characters.hpp"
#include "components/character_state.hpp"
#include "components/collisions.hpp"
#include "components/common.hpp"
#include "components/props.hpp"
#include "components/sprite.hpp"
#include <cmath>
#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>
#include <fmt/printf.h>
#include <entt.hpp>
#include "components/traits.hpp"
#include "keyinput.hpp"
#include "assets/asset_manager.hpp"
#include "gui/inspector.hpp"
#include "components/velocity.hpp"
#include "components/sprite.hpp"
#include "components/player.hpp"
#include "components/relations.hpp"
#include "components/props.hpp"

void load_resources(an::AssetManager &asset_manager) {
    using T = an::TextureEnum;
    using S = an::SoundEnum;

    auto player_img = an::load_asset(LoadImage, "player/player-test.png");
    auto test_tile = an::load_asset(LoadImage, "map/test-tile.png");
    asset_manager.register_texture(player_img, T::PLAYER_TEXTURE);
    asset_manager.register_texture(test_tile, T::TEST_TILE);

    asset_manager.register_texture(an::load_asset(LoadImage, "player/player_man.png"), T::BASE_CHARACTER);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/player_man_hair.png"), T::CHARACTER_HAIR, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/player_man_top.png"), T::CHARACTER_SHIRT, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/player_man_bottom.png"), T::CHARACTER_PANTS, 64,
                                   72);

    asset_manager.register_texture(an::load_asset(LoadImage, "props/bench.png"), T::BENCH);
    asset_manager.register_texture(an::load_asset(LoadImage, "props/lamp.png"), T::LAMP);
    asset_manager.register_texture(an::load_asset(LoadImage, "props/tree.png"), T::TREE);
    asset_manager.register_texture(an::load_asset(LoadImage, "props/rock.png"), T::ROCK);
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
    InitAudioDevice();
}

auto main() -> int {
    // setup
    setup_raylib();

    rlImGuiSetup(true);
    auto registry = entt::registry();
    an::init_tint_shader(registry);
    auto &key_manager = registry.ctx().emplace<an::KeyManager>();
    default_keys(key_manager);
    auto &asset_manager = registry.ctx().emplace<an::AssetManager>();
    load_resources(asset_manager);
    an::load_props(registry, an::load_asset(an::get_ifstream, "props.dat"));
    auto inspector = an::Inspector<an::LocalTransform, an::GlobalTransform, an::Sprite, an::Alive, an::Health,
                                   an::Player, an::Velocity, an::CharacterBody, an::StaticBody, an::Prop,
                                   an::FollowEntityCharState, an::EscapeCharState, an::AvoidTraitComponent,
                                   an::ShakeTraitComponent, an::FollowPathState, an::RandomWalkState>(&registry);

    key_manager.subscribe(an::KeyboardEvent::PRESS, KEY_N, [&]() { an::save_props(registry); });
    key_manager.subscribe(an::KeyboardEvent::PRESS, KEY_Q, [&]() { an::spawn_prop(registry); });
    // camera
    registry.ctx().emplace<Camera2D>(Vector2((float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2), Vector2(), 0.f,
                                     2.f);

    auto entity = registry.create();
    an::emplace<an::Sprite>(registry, entity, an::TextureEnum::TEST_TILE);
    // player
    [[maybe_unused]] auto player = an::make_player(registry);
    // shader
    auto base_shader = an::load_asset(LoadShader, "shaders/base.vs", "shaders/base.fs");

    an::emplace<an::ShaderComponent>(registry, player, base_shader);
    // an::emplace<an::Sprite>(registry, entity);

    // test char collider
    auto test_char_collider = registry.create();
    an::emplace<an::GlobalTransform>(registry, test_char_collider);
    an::emplace<an::CharacterBody>(registry, test_char_collider, Vector2(), 50.f);
    an::emplace<an::AvoidTraitComponent>(registry, test_char_collider, an::PropType::TREE, 100.f, 100.f);
    // an::emplace<an::ShakeTraitComponent>(registry, test_char_collider, an::PropType::TREE, 100.f, 1.f);
    an::emplace<an::FollowEntityCharState>(registry, test_char_collider, player, INFINITY, 10.f);

    // test character generator
    // auto char_gen = an::CharacterGenerator(0, 5);
    //
    // for(const auto &traits : char_gen.get_original_character_traits()) {
    //     const auto character = an::make_character(registry, traits);
    // }

    auto char_gen = an::CharacterGenerator(0, 50);

    for (const auto &traits : char_gen.get_original_character_traits()) {
        const auto character = an::make_character(registry, traits);
        auto &local_transform = registry.get<an::LocalTransform>(character);
        float x_r = ((float)rand() / RAND_MAX) * 2.f - 1.f;
        float y_r = ((float)rand() / RAND_MAX) * 2.f - 1.f;
        local_transform.transform.position = Vector2{x_r * 500.f, y_r * 500.f};
        an::emplace<an::RandomWalkState>(registry, character, 100.f, local_transform.transform.position, 1.f);
    }

    while (!WindowShouldClose()) {
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        an::notify_keyboard_press_system(key_manager);
        an::destroy_unparented(registry);
        an::propagate_parent_transform(registry);
        an::update_player(registry, player);
        an::update_props(registry);

        // Characters systems
        an::trait_systems(registry);
        an::character_states_systems(registry);

        an::move_things(registry);

        auto pos = registry.get<an::GlobalTransform>(player);
        registry.ctx().get<Camera2D>().target = pos.transform.position;

        for (int i = 0; i < 4; i++) {
            an::static_vs_character_collision_system(registry);
            an::character_vs_character_collision_system(registry);
            an::propagate_parent_transform(registry);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        // ======================================
        // DRAW SYSTEMS
        // ======================================

        BeginMode2D(registry.ctx().get<Camera2D>());

        an::render_drawables(registry);
        an::debug_draw_bodies(registry);
        an::debug_trait_systems(registry);

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
