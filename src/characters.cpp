#include "characters.hpp"
#include "components/character_state.hpp"
namespace an {

entt::entity make_character(entt::registry &registry, const CharacterTraits &traits) {
    auto entity = registry.create();
    an::emplace<an::Character>(registry, entity);
    an::emplace<an::CharacterStateMachine>(registry, entity);
    an::emplace<an::IdleState>(registry, entity);
    an::emplace<an::Animation>(registry, entity, 0.1f, 0.f, 0u, 4u);
    an::emplace<Velocity>(registry, entity, 0.f, 0.f);

    an::emplace_character_sprite(registry, entity, TextureEnum::BASE_CHARACTER, traits.hair_archetype,
                                 traits.shirt_archetype, traits.pants_archetype);
    auto &sprite = std::get<CharacterSprite>(registry.get<Drawable>(entity).sprite);

    an::emplace<an::CharacterBody>(registry, entity, Vector2{0.0f, 25.f}, 10.f, get_uniform_float());

    sprite.hair_color = traits.hair_color.color;
    sprite.shirt_color = traits.shirt_color.color;
    sprite.pants_color = traits.pants_color.color;

    return entity;
}
} // namespace an
