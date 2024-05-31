#pragma once
#include <entt.hpp>
#include <raylib.h>
#include "common.hpp"
#include "assets/asset_manager.hpp"
#include "relations.hpp"
#include "sprite.hpp"
namespace an {
enum class ParticleType : uint8_t { DRUNK, STINKY_CHEESE };
enum class ParticleAnimationType :uint8_t {NONE,SPIN_R,SPIN_L,SHAKE};
struct ParticleTemplate {
    ParticleType type;//
    ParticleAnimationType animation_type;//
    uint16_t duration;//
    uint8_t amount;//
    Vector2 dimensions;
    uint8_t animation_speed;//
    bool can_flip_x;//
    bool can_flip_y;//
};
struct ParticleEmitter {
    ParticleTemplate p_template;
    float life_time;//
    float time;//
    float spawn_time;//

};
struct Particle{
    float time;
    ParticleAnimationType a_type;
    uint8_t animation_speed;
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
auto make_particle(ParticleType particle,uint16_t duration =2000,uint8_t amount=10,Vector2 dimensions={20,20},uint8_t animation_speed = 100, ParticleAnimationType animation_type=ParticleAnimationType::NONE,bool can_flip_x = false,bool can_flip_y = true)-> ParticleTemplate {
    ParticleTemplate prefab;
    prefab.type = particle;
    prefab.duration = duration;
    prefab.amount = amount;
    prefab.dimensions = dimensions;
    prefab.animation_speed = animation_speed;
    prefab.animation_type = animation_type;
    prefab.can_flip_x = can_flip_x;
    prefab.can_flip_y = can_flip_y;
    return prefab;
}
void emit_particles(entt::registry &registry,entt::entity entity, ParticleTemplate particle_template,float life_time = -1){
    emplace<ParticleEmitter>(registry, entity);
    auto &emitter = registry.get<ParticleEmitter>(entity);
    emitter.life_time = life_time;
    emitter.p_template = particle_template;
}
void update_particle_system(entt::registry& registry){
    auto view = registry.view<ParticleEmitter,GlobalTransform>();
    for(auto &&[entity,emitter,transform]:view.each()){
        float delta = GetFrameTime();
        emitter.time +=delta;
        if(emitter.life_time<=delta){
            registry.erase<ParticleEmitter>(entity);
            continue;
        }
        emitter.spawn_time += delta;
        if(1.0/(float)emitter.p_template.amount<=emitter.spawn_time){
            emitter.spawn_time =0;
            auto pa_t = registry.create();
            emplace<Sprite>(registry,pa_t, get_particle_texture(emitter.p_template.type));
            auto &particle = registry.emplace<Particle>(entity);
            particle.time = emitter.p_template.duration;
            particle.a_type = emitter.p_template.animation_type;
            auto &drawable = registry.get<Drawable>(entity);
            auto &pa_transform = registry.get<LocalTransform>(entity);
            if(emitter.p_template.can_flip_x,random()%2==0){
                std::visit([](auto& sprite) { sprite.flip_v =true; }, drawable.sprite);
            }
            if(emitter.p_template.can_flip_y,random()%2==0){
                std::visit([](auto& sprite) { sprite.flip_h =true; }, drawable.sprite);
            }
            pa_transform.transform.position.x = transform.transform.position.x+static_cast<float>(random() % ((long)(2 * emitter.p_template.dimensions.x))) -emitter.p_template.dimensions.x;
            pa_transform.transform.position.y = transform.transform.position.y+static_cast<float>(random() % ((long)(2 * emitter.p_template.dimensions.y))) -emitter.p_template.dimensions.y;
            //つづく

        }

    }
}
} // namespace an