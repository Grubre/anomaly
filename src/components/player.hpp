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
    float bullet_speed = 1000.f;
    float to_next_shot = shooting_speed;

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

void update_bullets(entt::registry &registry);

void make_player_bullet(entt::registry &registry, Vector2 start, Vector2 dir, float speed, entt::entity player);

void player_shooting(entt::registry &registry, entt::entity &entity);

} // namespace an
