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
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};
struct Particle {
    float time;
    ParticleAnimationType a_type;
    float animation_speed;
    static constexpr auto name = "Particle";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};
TextureEnum get_particle_texture(const ParticleType type);
auto make_particle(ParticleType particle, float duration = 1, uint8_t amount = 10, Vector2 dimensions = {20, 20},
                   Vector2 scale = {1, 1}, float animation_speed = 100,
                   ParticleAnimationType animation_type = ParticleAnimationType::NONE, bool can_flip_x = false,
                   bool can_flip_y = true) -> ParticleTemplate;
void emit_particles(entt::registry &registry, entt::entity entity, ParticleTemplate particle_template,
                    float life_time = 5, Vector2 offset = {0, 0});
void update_particle_system(entt::registry &registry);
} // namespace an
