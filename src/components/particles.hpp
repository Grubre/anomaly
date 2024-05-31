#pragma once
#include <entt.hpp>
#include <raylib.h>
#include "common.hpp"
#include "assets/asset_manager.hpp"
#include "relations.hpp"
#include "sprite.hpp"
namespace an {
enum class ParticleType : uint8_t { DRUNK, STINKY_CHEESE };
enum class ParticleAnimationType : uint8_t { NONE, SPIN_R, SPIN_L, SHAKE, DECAY };
struct ParticleTemplate {
    ParticleType type;
    ParticleAnimationType animation_type;
    Vector2 scale;
    float duration;
    uint8_t amount;
    Vector2 dimensions;
    float animation_speed;
    bool can_flip_x;
    bool can_flip_y;
};
struct ParticleEmitter {
    ParticleTemplate p_template;
    float life_time;
    float time;
    float spawn_time;
    Vector2 offset;
    static constexpr auto name = "ParticleEmitter";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Value("Life Time", life_time);
        ImGui::Value("Time", time);
        ImGui::Value("Spawn Time", spawn_time);
    }
};
struct Particle {
    float time;
    ParticleAnimationType a_type;
    float animation_speed;
    static constexpr auto name = "Particle";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Value("Time", time);
        ImGui::Value("Animation Speed", animation_speed);
    }
};
TextureEnum get_particle_texture(const ParticleType type) {
    switch (type) {
    case ParticleType::DRUNK:
        return TextureEnum::DRUNK_PARTICLE;
    case ParticleType::STINKY_CHEESE:
        return TextureEnum::STINKY_PARTICLE;
    default:
        return TextureEnum::CNT;
    }
}
auto make_particle(ParticleType particle, float duration = 1, uint8_t amount = 10, Vector2 dimensions = {20, 20},
                   Vector2 scale = {1, 1}, float animation_speed = 100,
                   ParticleAnimationType animation_type = ParticleAnimationType::NONE, bool can_flip_x = false,
                   bool can_flip_y = true) -> ParticleTemplate {
    ParticleTemplate prefab;
    prefab.type = particle;
    prefab.duration = duration;
    prefab.amount = amount;
    prefab.dimensions = dimensions;
    prefab.animation_speed = animation_speed;
    prefab.animation_type = animation_type;
    prefab.can_flip_x = can_flip_x;
    prefab.can_flip_y = can_flip_y;
    prefab.scale = scale;
    return prefab;
}
void emit_particles(entt::registry &registry, entt::entity entity, ParticleTemplate particle_template,
                    float life_time = 5, Vector2 offset = {0, 0}) {
    emplace<ParticleEmitter>(registry, entity);
    auto &emitter = registry.get<ParticleEmitter>(entity);
    emitter.life_time = life_time;
    emitter.p_template = particle_template;
    emitter.offset = offset;
}
void update_particle_system(entt::registry &registry) {
    auto view = registry.view<ParticleEmitter, GlobalTransform>();
    for (auto &&[entity, emitter, transform] : view.each()) {
        float delta = GetFrameTime();
        emitter.time += delta;
        if (emitter.life_time <= emitter.time) {
            registry.erase<ParticleEmitter>(entity);
            continue;
        }
        emitter.spawn_time += delta;
        if (1.0 / (float)emitter.p_template.amount <= emitter.spawn_time) {
            emitter.spawn_time = 0;
            auto pa_t = registry.create();
            emplace<Sprite>(registry, pa_t, get_particle_texture(emitter.p_template.type));
            emplace<Particle>(registry, pa_t);
            auto &particle = registry.get<Particle>(pa_t);
            particle.time = emitter.p_template.duration;
            particle.a_type = emitter.p_template.animation_type;
            particle.animation_speed = emitter.p_template.animation_speed;
            auto &drawable = registry.get<Drawable>(pa_t);
            auto &pa_transform = registry.get<LocalTransform>(pa_t);
            if (emitter.p_template.can_flip_x, random() % 2 == 0) {
                std::visit([](auto &sprite) { sprite.flip_v = true; }, drawable.sprite);
            }
            if (emitter.p_template.can_flip_y, random() % 2 == 0) {
                std::visit([](auto &sprite) { sprite.flip_h = true; }, drawable.sprite);
            }
            pa_transform.transform.position.x =
                emitter.offset.x + transform.transform.position.x +
                static_cast<float>(random() % ((long)(2 * emitter.p_template.dimensions.x))) -
                emitter.p_template.dimensions.x;
            pa_transform.transform.position.y =
                emitter.offset.y + transform.transform.position.y +
                static_cast<float>(random() % ((long)(2 * emitter.p_template.dimensions.y))) -
                emitter.p_template.dimensions.y;
            pa_transform.transform.scale = emitter.p_template.scale;
        }
    }
    auto p_view = registry.view<Particle, LocalTransform>();
    for (auto &&[entity, pa, transform] : p_view.each()) {
        pa.time -= GetFrameTime();
        if (pa.time <= 0) {
            registry.destroy(entity);
            continue;
        }
        switch (pa.a_type) {
        case ParticleAnimationType::SPIN_R:
            transform.transform.rotation += GetFrameTime() * pa.animation_speed;
            break;
        case ParticleAnimationType::SPIN_L:
            transform.transform.rotation -= GetFrameTime() * pa.animation_speed;
            break;
        case ParticleAnimationType::SHAKE:
            transform.transform.position.x += GetFrameTime() * pa.animation_speed * (random() % 2 == 0 ? 1.0f : -1.0f);
            transform.transform.position.y += GetFrameTime() * pa.animation_speed * (random() % 2 == 0 ? 1.0f : -1.0f);
            break;
        case ParticleAnimationType::DECAY:
            transform.transform.scale.x -= GetFrameTime() * pa.animation_speed;
            transform.transform.scale.y -= GetFrameTime() * pa.animation_speed;
            if (transform.transform.scale.x <= 0 || transform.transform.scale.y <= 0) {
                registry.destroy(entity);
                continue;
            }
            break;
        default:
            break;
        }
    }
}
} // namespace an