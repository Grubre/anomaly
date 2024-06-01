#include "player.hpp"
#include <cmath>
#include "assets/asset_manager.hpp"
#include "characters.hpp"
#include "components/character_state.hpp"
#include "components/equipment.hpp"
#include "components/sprite.hpp"
#include "corpse.h"
namespace an {

void Health::inspect(entt::registry &registry, entt::entity entity) {
    static constexpr auto minimum = 0;
    ImGui::DragInt("Health", &health, 1, minimum, maxHealth);
}

void deal_damage(entt::registry &registry, entt::entity &entity, int amount) {
    auto &health = registry.get<Health>(entity);
    health.health -= amount;
    if (health.health <= 0) {
        registry.remove<Alive>(entity);
    }
}
auto make_player(entt::registry &registry) -> entt::entity {
    const auto entity = registry.create();
    registry.emplace<an::IdleState>(entity);
    registry.emplace<an::Animation>(entity, 0.1f, 0.f, 0u, 4u);
    registry.emplace<an::Equipment>(entity);
    an::emplace<an::Player>(registry, entity);
    an::emplace<an::Selected>(registry, entity);
    return entity;
}
void update_player(entt::registry &registry, entt::entity &entity) {
    auto &key_manager = registry.ctx().get<an::KeyManager>();
    auto &player = registry.get<Player>(entity);
    auto &vel = registry.get<Velocity>(entity);
    // movement
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
void update_bullets(entt::registry &registry,entt::entity player) {
    auto pl = registry.get<Player>(player);
    if(pl.aiming_state && pl.cooldown<= 0.f){
        auto pl_trans = registry.get<GlobalTransform>(player);
        Vector2 pos = GetScreenToWorld2D(GetMousePosition(), registry.ctx().get<Camera2D>());
        pos = Vector2Subtract(pos,pl_trans.transform.position);
        pos = Vector2Scale(pos,10);
        pos = Vector2Add(pos,pl_trans.transform.position);
        DrawLineEx(pl_trans.transform.position, pos,2.2f,RED);
    }
    auto view = registry.view<Bullet, Velocity>();
    for (auto &&[entity, bullet, vel] : view.each()) {
        bullet.time_left -= GetFrameTime();

        vel = Vector2Scale(bullet.start_vel, bullet.time_left / bullet.total_time);

        if (bullet.time_left <= 0.f) {
            registry.destroy(entity);
        }
    }
    auto view2 = registry.view<RealBullet, Velocity, LocalTransform>();
    for (auto &&[entity, bullet, vel, trans] : view2.each()) {
        bullet.time_left -= GetFrameTime();

        vel = Vector2Scale(bullet.start_vel, bullet.time_left / bullet.total_time);

        if (bullet.time_left <= 0.f) {
            registry.destroy(entity);
        }
        auto view3 = registry.view<Character, GlobalTransform>();

        for (auto &&[cha, cha_tran] : view3.each()) {
            if (Vector2Distance(trans.transform.position, cha_tran.transform.position) < 20) {
                trans.transform.position = Vector2{-1000, -1000};
                make_corpse(registry, cha_tran.transform.position);
                registry.destroy(cha);
            }
        }
    }
}
void make_player_bullet(entt::registry &registry, Vector2 start, Vector2 dir, float speed, entt::entity player) {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::normal_distribution<float> d{0.f, PI / 32.f};

    auto bullet = registry.create();

    auto spread_angle = d(gen);
    dir = Vector2Rotate(dir, spread_angle);

    emplace<GlobalTransform>(registry, bullet);
    emplace<CharacterBody>(registry, bullet, Vector2(), 10.f, 10.f);
    emplace<Velocity>(registry, bullet, dir.x * speed, dir.y * speed);
    emplace<Bullet>(registry, bullet, dir.x * speed, dir.y * speed, player);
    emplace<Sprite>(registry, bullet, TextureEnum::DROPS);
    auto &tr = registry.get<LocalTransform>(bullet);
    tr.transform.rotation = std::atan2(dir.x, -dir.y);
    tr.transform.position = Vector2Add(start, Vector2Scale(dir, 30.f));
}
void make_player_bullet_real(entt::registry &registry, Vector2 start, Vector2 dir, float speed, entt::entity player) {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::normal_distribution<float> d{0.f, PI / 100.f};

    auto bullet = registry.create();

    auto spread_angle = d(gen);
    dir = Vector2Rotate(dir, spread_angle);

    emplace<GlobalTransform>(registry, bullet);
    emplace<Velocity>(registry, bullet, dir.x * speed, dir.y * speed);
    emplace<RealBullet>(registry, bullet, dir.x * speed, dir.y * speed, player);
    emplace<Sprite>(registry, bullet, TextureEnum::BULLET);
    auto &tr = registry.get<LocalTransform>(bullet);
    tr.transform.rotation = std::atan2(dir.x, -dir.y);
    tr.transform.scale = Vector2{8.0f, 8.0f};
    tr.transform.position = Vector2Add(start, Vector2Scale(dir, 30.f));
}
void player_shooting(entt::registry &registry, entt::entity &entity) {
    auto &player = registry.get<Player>(entity);
    player.cooldown-= GetFrameTime();
    if(registry.all_of<ShowUI>(entity)){
        return;
    }
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        if (player.aiming_state ) {
            if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)|| player.cooldown > 0.f) {
                return;
            }
            auto mouse_pos = GetScreenToWorld2D(GetMousePosition(), registry.ctx().get<Camera2D>());

            auto &transform = registry.get<GlobalTransform>(entity);

            auto dir = Vector2Normalize(Vector2Subtract(mouse_pos, transform.transform.position));

            make_player_bullet_real(registry, transform.transform.position, dir, player.alt_bullet_speed, entity);

            auto &player_local = registry.get<LocalTransform>(entity);
            player.cooldown = 3.1f;
            player_local.transform.position = Vector2Subtract(player_local.transform.position, Vector2Scale(dir, 1.f));
            return;
        }
        player.to_next_shot -= GetFrameTime();
        while (player.to_next_shot <= 0.f) {
            auto mouse_pos = GetScreenToWorld2D(GetMousePosition(), registry.ctx().get<Camera2D>());

            auto &transform = registry.get<GlobalTransform>(entity);

            auto dir = Vector2Normalize(Vector2Subtract(mouse_pos, transform.transform.position));

            make_player_bullet(registry, transform.transform.position, dir, player.bullet_speed, entity);

            player.to_next_shot += player.shooting_speed;

            auto &player_local = registry.get<LocalTransform>(entity);

            player_local.transform.position = Vector2Subtract(player_local.transform.position, Vector2Scale(dir, 1.f));
        }
    }
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        player.aiming_state = true;
    } else {
        player.aiming_state = false;
    }
}
void Player::inspect(entt::registry &registry, entt::entity entity) {
    static constexpr auto minimum = 0;
    ImGui::DragFloat("Speed", &speed, 0.5f, minimum, 1000);
    ImGui::DragFloat("Bullet Speed", &bullet_speed, 0.5f, minimum, 10000);
    ImGui::DragFloat("Shooting Speed", &shooting_speed, 0.001f, minimum, 10);
}
template <> void emplace<Player>(entt::registry &registry, entt::entity entity) {
    emplace<Velocity>(registry, entity);
    emplace<Health>(registry, entity);
    emplace<Alive>(registry, entity);
    emplace<DebugName>(registry, entity, "Player");

    emplace_sprite(registry,entity, TextureEnum::MAIN_CHAR);

    emplace<CharacterBody>(registry, entity, Vector2{0.0f, 10.f}, 10.f);
    safe_emplace<Player>(registry, entity);
}
} // namespace an
