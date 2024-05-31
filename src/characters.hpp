#pragma once

#include "character_generator.hpp"
#include "components/common.hpp"
#include <cstdlib>
#include <entt.hpp>

namespace an {

struct Character {
    static constexpr auto name = "Character";
    static void inspect() { ImGui::Text("EPIC NPC"); }
};

inline entt::entity make_character(entt::registry &registry, const CharacterTraits &traits) {
    auto entity = registry.create();
    an::emplace<an::Character>(registry, entity);
    an::emplace_character_sprite(registry, entity, TextureEnum::BASE_CHARACTER, TextureEnum::CHARACTER_HAIR,
                                 TextureEnum::CHARACTER_SHIRT, TextureEnum::CHARACTER_PANTS);
    auto &sprite = std::get<CharacterSprite>(registry.get<Drawable>(entity).sprite);

    an::emplace<an::CharacterBody>(registry, entity, Vector2{}, 10.f, get_uniform_float());

    sprite.hair_color = traits.hair_color.color;
    sprite.shirt_color = traits.shirt_color.color;
    sprite.pants_color = traits.pants_color.color;

    return entity;
}

inline void turn_into_anomaly(entt::registry &registry, entt::entity character, const AnomalyTraits &traits) {}

} // namespace an
