#pragma once

#include "assets/asset_manager.hpp"
#include "common.hpp"
#include "components/sprite.hpp"
#include "components/collisions.hpp"
#include "gui/inspector.hpp"
#include "velocity.hpp"
#include "keyinput.hpp"
#include <cstdlib>
#include <fmt/printf.h>
#include <imgui.h>
#include <random>
#include <raylib.h>
#include <raymath.h>

namespace an {
struct Alive {
    static constexpr auto name = "Alive";
    static void inspect() { ImGui::Text("Entity Alive :>"); }
};

struct Health {
    static constexpr auto name = "Health";
    int maxHealth = 100;
    int health = 100;
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};

void deal_damage(entt::registry &registry, entt::entity &entity, int amount);

struct Player {
    static constexpr auto name = "Player";
    float speed = 256;

    float shooting_speed = 0.05f;
    float alt_bullet_speed = 2000.f;
    float bullet_speed = 1000.f;
    float to_next_shot = shooting_speed;
    bool aiming_state= false;

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};

template <> void emplace<Player>(entt::registry &registry, entt::entity entity);

[[nodiscard]] auto make_player(entt::registry &registry) -> entt::entity;
void update_player(entt::registry &registry, entt::entity &entity);

struct Bullet {
    Vector2 start_vel;
    entt::entity player;
    float total_time = 0.5f;
    float time_left = total_time;
};
struct RealBullet {
    Vector2 start_vel;
    entt::entity player;
    float total_time = 0.5f;
    float time_left = total_time;
};

struct SleepingPlayer {
    float time_left {5.f};

    static constexpr auto name = "Sleeping";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat("Time Left", &time_left);
    }
};

inline void sleep_player(entt::registry &registry) {
    auto view = registry.view<SleepingPlayer>();

    for (auto &&[entity, sleep] : view.each()) {
        sleep.time_left -= GetFrameTime();
        if (sleep.time_left <= 0.f) {
            registry.remove<SleepingPlayer>(entity);
            auto& local = registry.get<LocalTransform>(entity);
            local.transform.rotation = 0.f;
        }
    }
}

void update_bullets(entt::registry &registry,entt::entity player);

void make_player_bullet(entt::registry &registry, Vector2 start, Vector2 dir, float speed, entt::entity player);

void player_shooting(entt::registry &registry, entt::entity &entity);

} // namespace an
