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
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        static constexpr auto minimum = 0;
        ImGui::DragInt("Health", &health, 1, minimum, maxHealth);
    }
};

void deal_damage(entt::registry &registry, entt::entity &entity, int amount) {
    auto &health = registry.get<Health>(entity);
    health.health -= amount;
    if (health.health <= 0) {
        registry.remove<Alive>(entity);
    }
}

struct Player {
    static constexpr auto name = "Player";
    float speed = 256;

    float shooting_speed = 0.05f;
    float bullet_speed = 1000.f;
    float to_next_shot = shooting_speed;

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        static constexpr auto minimum = 0;
        ImGui::DragFloat("Speed", &speed, 0.5f, minimum, 1000);
        ImGui::DragFloat("Bullet Speed", &bullet_speed, 0.5f, minimum, 10000);
        ImGui::DragFloat("Shooting Speed", &shooting_speed, 0.001f, minimum, 10);
    }
};

template <> inline void emplace<Player>(entt::registry &registry, entt::entity entity) {
    emplace<Velocity>(registry, entity);
    emplace<Health>(registry, entity);
    emplace<Alive>(registry, entity);
    emplace<DebugName>(registry, entity, "Player");

    emplace_character_sprite(registry, entity, TextureEnum::BASE_CHARACTER, TextureEnum::CHARACTER_HAIR, TextureEnum::CHARACTER_SHIRT, TextureEnum::CHARACTER_PANTS);

    emplace<CharacterBody>(registry, entity, Vector2{}, 10.f);
    safe_emplace<Player>(registry, entity);
}

[[nodiscard]] auto make_player(entt::registry &registry) -> entt::entity {
    const auto entity = registry.create();
    an::emplace<an::Player>(registry, entity);
    return entity;
}
void update_player(entt::registry &registry, entt::entity &entity) {
    auto &key_manager = registry.ctx().get<an::KeyManager>();
    auto &player = registry.get<Player>(entity);
    auto &vel = registry.get<Velocity>(entity);
    //movement
    Vector2 movement = {0, 0};
    if (IsKeyDown(key_manager.get_key(KeyEnum::MOVE_UP))) {
        movement.y -= 1;
    }
    if (IsKeyDown(key_manager.get_key(KeyEnum::MOVE_DOWN))) {
        movement.y += 1;
    }
    if (IsKeyDown(key_manager.get_key(KeyEnum::MOVE_LEFT))) {
        movement.x -= 1;
    }
    if (IsKeyDown(key_manager.get_key(KeyEnum::MOVE_RIGHT))) {
        movement.x += 1;
    }
    vel = Vector2Scale(Vector2Normalize(movement), player.speed);
}

struct Bullet {
    Vector2 start_vel;
    float total_time = 0.5f;
    float time_left = total_time;
};

inline void update_bullets(entt::registry &registry) {
    auto view = registry.view<Bullet, Velocity>();

    for (auto &&[entity, bullet, vel] : view.each()) {
        bullet.time_left -= GetFrameTime();

        vel = Vector2Scale(bullet.start_vel, bullet.time_left / bullet.total_time);

        if (bullet.time_left <= 0.f) {
            registry.destroy(entity);
        }
    }
}

inline void make_player_bullet(entt::registry &registry, Vector2 start, Vector2 dir, float speed) {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::normal_distribution<float> d{ 0.f, PI / 32.f };

    auto bullet = registry.create();

    auto spread_angle = d(gen);
    dir = Vector2Rotate(dir, spread_angle);

    emplace<GlobalTransform>(registry, bullet);
    emplace<CharacterBody>(registry, bullet, Vector2(), 10.f, 10.f);
    emplace<Velocity>(registry, bullet, dir.x * speed, dir.y * speed);
    emplace<Bullet>(registry, bullet, dir.x * speed, dir.y * speed);

    auto& tr = registry.get<LocalTransform>(bullet);

    tr.transform.position = Vector2Add(start, Vector2Scale(dir, 30.f));
}

inline void player_shooting(entt::registry &registry, entt::entity &entity) {

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        auto &player = registry.get<Player>(entity);

        player.to_next_shot -= GetFrameTime();

        while (player.to_next_shot <= 0.f) {
            auto mouse_pos = GetScreenToWorld2D(GetMousePosition(),registry.ctx().get<Camera2D>());

            auto &transform = registry.get<GlobalTransform>(entity);

            auto dir = Vector2Normalize(Vector2Subtract(mouse_pos, transform.transform.position));

            make_player_bullet(registry, transform.transform.position, dir, player.bullet_speed);

            player.to_next_shot += player.shooting_speed;

            auto &player_local = registry.get<LocalTransform>(entity);

            player_local.transform.position = Vector2Subtract(player_local.transform.position, Vector2Scale(dir, 1.f));
        }
    }
}

} // namespace an
