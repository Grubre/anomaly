#include "corpse.h"
#include "sprite.hpp"
auto an::make_cropse(entt::registry &registry, Vector2 pos) -> entt::entity {
    auto entity = registry.create();
    emplace<Sprite>(registry, entity, TextureEnum::CORPSE);
    return entity;
}
