#include "assets/asset_manager.hpp"
#include "common.hpp"
#include "components/sprite.hpp"
#include "components/collisions.hpp"
#include "gui/inspector.hpp"
#include "velocity.hpp"
#include "keyinput.hpp"
#include <fmt/printf.h>

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
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        static constexpr auto minimum = 0;
        ImGui::DragFloat("Speed", &speed, 0.5f, minimum, 1000);
    }
};

template <> inline void emplace<Player>(entt::registry &registry, entt::entity entity) {
    emplace<Velocity>(registry, entity);
    emplace<Health>(registry, entity);
    emplace<Alive>(registry, entity);
    emplace<DebugName>(registry, entity, "Player");
    emplace<Sprite>(registry, entity, TextureEnum::PLAYER_TEXTURE);
    safe_emplace<Player>(registry, entity);
}

[[nodiscard]] auto make_player(entt::registry &registry) -> entt::entity {
    const auto entity = registry.create();
    an::emplace<an::Player>(registry, entity);
    an::emplace<CharacterBody>(registry, entity);
    using KE = an::KeyboardEvent;
    auto &vel = registry.get<Velocity>(entity);
    auto &manager = registry.ctx().get<an::KeyManager>();
    auto &player = registry.get<Player>(entity);
    manager.subscribe(KE::DOWN, KeyEnum::MOVE_DOWN, [&]() { vel.y += player.speed; });
    manager.subscribe(KE::DOWN, KeyEnum::MOVE_UP, [&]() { vel.y -= player.speed; });
    manager.subscribe(KE::DOWN, KeyEnum::MOVE_LEFT, [&]() { vel.x -= player.speed; });
    manager.subscribe(KE::DOWN, KeyEnum::MOVE_RIGHT, [&]() { vel.x += player.speed; });
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
    vel = Vector2Scale(Vector2Normalize(movement), player.speed*GetFrameTime());
}
} // namespace an
