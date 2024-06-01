#include "corpse.h"
#include "assets/asset_manager.hpp"
#include "common.hpp"
#include "sprite.hpp"
auto an::make_corpse(entt::registry &registry, Vector2 pos) -> entt::entity {
    auto entity = registry.create();
    emplace<Sprite>(registry, entity, TextureEnum::CORPSE);
    registry.emplace<Corpse>(entity);
    auto &transform =registry.get<LocalTransform>(entity);
    transform.transform.position = pos;
    return entity;
}
