#include "assets/assets_loader.hpp"
#include "character_generator.hpp"
#include "characters.hpp"
#include "components/buildings.hpp"
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
#include "components/player.hpp"
#include "components/relations.hpp"
#include "components/walk_area.hpp"
#include "components/particles.hpp"
#include "components/city.hpp"
#include "gui/inspect_window.hpp"
#include "components/marker.hpp"

#include "components/buildings.hpp"
void load_resources(an::AssetManager &asset_manager) {
    using T = an::TextureEnum;
    using S = an::SoundEnum;

    const auto load_image = [&](const char *path, an::TextureEnum id) {
        asset_manager.register_texture(an::load_asset(LoadImage, path), id);
    };
    // tmp
    load_image("map/test-tile.png", T::TEST_TILE);
    // player
    asset_manager.register_texture(an::load_asset(LoadImage, "player/base_walk_animation.png"), T::BASE_CHARACTER, 64,
                                   72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/hair_1.png"), T::CHARACTER_HAIR_1, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/hair_2.png"), T::CHARACTER_HAIR_2, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/hair_4.png"), T::CHARACTER_HAIR_4, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/top_1.png"), T::CHARACTER_SHIRT_1, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/top_2.png"), T::CHARACTER_SHIRT_2, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/top_4.png"), T::CHARACTER_SHIRT_4, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/bottom_1.png"), T::CHARACTER_PANTS_1, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/bottom_2.png"), T::CHARACTER_PANTS_2, 64, 72);
    asset_manager.register_texture(an::load_asset(LoadImage, "player/bottom_4.png"), T::CHARACTER_PANTS_4, 64, 72);
    // props
    load_image("props/bench.png", T::BENCH);
    load_image("props/lamp.png", T::LAMP);
    load_image("props/tree_1.png", T::TREE);
    load_image("props/rock.png", T::ROCK);
    // particles
    load_image("particles/drunk.png", T::DRUNK_PARTICLE);
    load_image("particles/smrodek.png", T::STINKY_PARTICLE);
    // ui
    load_image("ui/back_btn.png", T::B_BACK);
    load_image("ui/legit_btn.png", T::B_LEGIT);
    load_image("ui/sus_btn.png", T::B_SUS);
    load_image("ui/tlo.png", T::UI_BACKGROUND);
    // other
    load_image("other/marker.png", T::MARKER);
    load_image("other/bullet.png", T::BULLET);
    // map
    load_image("map/city-tile-NW.png", T::CITY_TILE_NW);
    load_image("map/city-tile-N1.png", T::CITY_TILE_N);
    load_image("map/city-tile-NE.png", T::CITY_TILE_NE);
    load_image("map/city-tile-W.png", T::CITY_TILE_W);
    load_image("map/city-tile-square.png", T::CITY_TILE_SQUARE);
    load_image("map/city-tile-E.png", T::CITY_TILE_E);

    load_image("map/city-tile-houses-N.png", T::CITY_HOUSES_N);
    load_image("map/city-tile-houses-N2.png", T::CITY_HOUSES_N2);
    load_image("map/city-tile-houses-NE.png", T::CITY_HOUSES_NE);
    load_image("map/city-tile-houses-NE2.png", T::CITY_HOUSES_NE2);
    load_image("map/city-tile-houses-NW.png", T::CITY_HOUSES_NW);
    load_image("map/city-tile-houses-NW2.png", T::CITY_HOUSES_NW2);
}

void default_keys(an::KeyManager &key_manager) {
    key_manager.assign_key(KEY_W, an::KeyEnum::MOVE_UP);
    key_manager.assign_key(KEY_S, an::KeyEnum::MOVE_DOWN);
    key_manager.assign_key(KEY_A, an::KeyEnum::MOVE_LEFT);
    key_manager.assign_key(KEY_D, an::KeyEnum::MOVE_RIGHT);
    key_manager.assign_key(KEY_E, an::KeyEnum::INTERACT);
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

namespace an {
struct Anomaly {
    static constexpr auto name = "Anomaly";
    static void inspect() { ImGui::Text("Is anomaly"); }
};
} // namespace an

void anomaly_traits_gui(an::ResolvedDay &day) {
    ImGui::Begin("Anomaly traits");
    ImGui::SeparatorText(fmt::format("Probable traits ({} of {})", day.num_used_probable_traits,
                                     std::variant_size<an::ProbableTrait>().value)
                             .c_str());
    for (auto &trait : day.anomaly_traits.probable_traits) {
        ImGui::Text("%s: ", probable_trait_name_to_str(trait).data());
        ImGui::SameLine();
        std::visit(entt::overloaded{
                       [&](const auto &color) {
                           ImGui::ColorButton("Color",
                                              ImVec4((float)color.color.r / 255.f, (float)color.color.g / 255.f,
                                                     (float)color.color.b / 255.f, 1.f));
                       },
                       [&](const an::Accessory &accessory) { ImGui::Text("Accessory: %d", accessory.accessory_num); },
                       [&](const an::ParticleTrait &particle) { ImGui::Text("Particle: %d", (int)particle.type); },
                   },
                   trait);
    }
    ImGui::SeparatorText("Guaranteed traits");
    for (auto &trait : day.anomaly_traits.guaranteed_traits) {
        ImGui::Text("%s", guaranteed_trait_to_str(trait).c_str());
    }
    ImGui::End();
}

auto create_connected_walk_areas(entt::registry &registry, uint32_t number) -> entt::entity {
    entt::entity entity{};
    an::WalkArea *prev_area = nullptr;

    for (auto i = 0u; i < number; i++) {
        entity = registry.create();
        const auto size = 200.f;
        an::emplace<an::WalkArea>(registry, entity, Vector2{size * (float)i, 0.f},
                                  Vector2{size * (float)(i + 1) - 1.f, size});
        auto *current_area = &registry.get<an::WalkArea>(entity);

        if (prev_area != nullptr) {
            an::connect_walk_areas(*current_area, *prev_area);
        }

        prev_area = current_area;
    }

    return entity;
}

auto gen_npcs(entt::registry &registry, an::WalkArea *walk_area, const an::DayConfig &config) -> an::ResolvedDay {
    const auto num_anomalies = 5;
    auto rd = std::random_device{};
    auto gen = std::mt19937{rd()};
    auto dis = std::uniform_int_distribution<std::uint32_t>{0, INT_MAX};
    auto char_gen = an::CharacterGenerator(dis(gen), 100);
    auto day = char_gen.new_day(config);

    auto i = 0u;
    for (const auto &traits : day.characters) {
        const auto character = an::make_character(registry, traits);
        auto &local_transform = registry.get<an::LocalTransform>(character);
        float x_r = an::get_uniform_float() * 2.f - 1.f;
        float y_r = an::get_uniform_float() * 2.f - 1.f;
        local_transform.transform.position = Vector2{x_r * 500.f, y_r * 500.f};

        an::emplace<an::RandomWalkState>(registry, character, 100.f, local_transform.transform.position, 1.f,
                                         walk_area);

        if (i < num_anomalies) {
            for (const auto &trait : day.anomaly_traits.guaranteed_traits) {
                std::visit(entt::overloaded{
                               [&](const an::Avoid &avoid) {
                                   an::emplace<an::AvoidTraitComponent>(registry, character, avoid.type, 100.f, 200.f);
                               },
                               [&](const an::TwitchNear &twitch) {
                                   an::emplace<an::ShakeTraitComponent>(registry, character, twitch.type, 100.f,
                                                                        5000.f);
                               },
                           },
                           trait);
            }
            an::emplace<an::Anomaly>(registry, character);
            an::emplace<an::DebugName>(registry, character, "Anomaly");
        } else {
            an::emplace<an::DebugName>(registry, character, "NPC");
        }

        i++;
    }

    return day;
}

auto main() -> int {
    // setup
    setup_raylib();

    rlImGuiSetup(true);
    auto registry = entt::registry();
    an::init_collision_controller(registry);
    an::init_tint_shader(registry);
    an::init_edge_detection_shader(registry);
    an::init_interrupted_state_and_menu_bridge(registry);
    auto &key_manager = registry.ctx().emplace<an::KeyManager>();
    default_keys(key_manager);
    auto &asset_manager = registry.ctx().emplace<an::AssetManager>();
    load_resources(asset_manager);
    an::load_props(registry, an::load_asset(an::get_ifstream, "props.dat"));
    auto inspector =
        an::Inspector<an::LocalTransform, an::GlobalTransform, an::Drawable, an::Alive, an::Health, an::Player,
                      an::Velocity, an::CharacterBody, an::StaticBody, an::Prop, an::AvoidTraitComponent,
                      an::ShakeTraitComponent, an::FollowPathState, an::RandomWalkState, an::WalkArea,
                      an::ParticleEmitter, an::Particle, an::Character, an::Marked, an::Interrupted, an::ShowUI,
                      an::Marker, an::CharacterStateMachine, an::WalkingState, an::IdleState>(&registry);

    key_manager.subscribe(an::KeyboardEvent::PRESS, KEY_N, [&]() { an::save_props(registry); });
    key_manager.subscribe(an::KeyboardEvent::PRESS, KEY_Q, [&]() { an::spawn_prop(registry); });
    // camera
    registry.ctx().emplace<Camera2D>(Vector2((float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2), Vector2(), 0.f,
                                     3.f);

    auto post_process_texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_SQUARE, Vector2(0.f, 0.f));
    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_NW, Vector2(-1.f, -1.f));
    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_N, Vector2(0.f, -1.f));
    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_NE, Vector2(1.f, -1.f));
    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_W, Vector2(-1.f, 0.f));
    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_E, Vector2(1.f, 0.f));
    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_SQUARE, Vector2(0.f, 1.f));
    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_SQUARE, Vector2(1.f, 1.f));
    an::make_city_tile(registry, an::TextureEnum::CITY_TILE_SQUARE, Vector2(-1.f, 1.f));

    an::make_building_l1(registry, Vector2(0.f, -1.f), an::TextureEnum::CITY_HOUSES_N);

    an::make_building_l2(registry, Vector2(0.f, -1.f), an::TextureEnum::CITY_HOUSES_N2);

    auto entity = registry.create();
    an::emplace<an::Sprite>(registry, entity, an::TextureEnum::TEST_TILE);
    // player
    [[maybe_unused]] auto player = an::make_player(registry);
    key_manager.subscribe(an::KeyboardEvent::PRESS, an::KeyEnum::INTERACT,
                          [&]() { an::check_nearby_npc(registry, player); });
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
    an::emplace<an::FollowEntityState>(registry, test_char_collider, player, INFINITY, 10.f);

    auto walk_area_entity = create_connected_walk_areas(registry, 3);
    auto *walk_area = &registry.get<an::WalkArea>(walk_area_entity);

    // test anomalies and npcs
    auto day = gen_npcs(registry, walk_area,
                        an::DayConfig{
                            .num_guaranteed_traits = 2,
                            .num_used_probable_traits = 3,
                            .num_anomalies = 5,
                        });
    an::propagate_parent_transform(registry);

    // particle
    auto drunk_p = an::make_particle(an::ParticleType::DRUNK, 3, 6, {20, 20}, {2, 2}, 5,
                                     an::ParticleAnimationType::SPIN_R, true, true);
    key_manager.subscribe(an::KeyboardEvent::PRESS, KEY_J, [&]() {
        an::emit_particles(registry, player, drunk_p, 5, {0, -5});
    });
    while (!WindowShouldClose()) {
        // ======================================
        // UPDATE SYSTEMS
        // ======================================
        an::notify_keyboard_press_system(key_manager);
        an::destroy_unparented(registry);
        an::update_player(registry, player);
        an::update_props(registry);
        an::player_shooting(registry, player);
        an::update_marker_system(registry, player);
        an::update_particle_system(registry);
        an::update_bullets(registry);

        // Characters systems
        an::trait_systems(registry);
        an::character_states_systems(registry);

        an::move_things(registry);
        an::propagate_parent_transform(registry);

        auto pos = registry.get<an::GlobalTransform>(player);
        registry.ctx().get<Camera2D>().target = pos.transform.position;

        for (int i = 0; i < 4; i++) {
            an::static_vs_character_collision_system(registry);
            an::character_vs_character_collision_system(registry);
            an::propagate_parent_transform(registry);
        }

        an::set_move_state_system(registry);
        an::update_character_animations(registry);

        BeginTextureMode(post_process_texture);
        an::hide_buildings_if_needed(registry, player);
        ClearBackground(RAYWHITE);
        // ======================================
        // DRAW SYSTEMS
        // ======================================

        BeginMode2D(registry.ctx().get<Camera2D>());

        an::render_city_tiles(registry);

        an::y_sort(registry);

        // an::visualize_walk_areas(registry);

        an::render_drawables(registry);
        an::debug_draw_bodies(registry);
        // an::debug_trait_systems(registry);

        // an::debug_buildings(registry);

        EndMode2D();

        EndTextureMode();

        BeginDrawing();

        DrawTextureRec(
            post_process_texture.texture,
            (Rectangle){0, 0, (float)post_process_texture.texture.width, (float)-post_process_texture.texture.height},
            (Vector2){0, 0}, WHITE);

        // ======================================
        // DRAW GUI
        // ======================================

        rlImGuiBegin();
        an::update_ui(registry, player);
        // ImGui::ShowDemoWindow();
        inspector.draw_gui();
        anomaly_traits_gui(day);
        rlImGuiEnd();

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseAudioDevice();
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
