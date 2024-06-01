#include "corpse.hpp"
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
an::WinCondition an::check_win_condition(entt::registry &registry) {
    auto ret = NONE;
    if(registry.view<Anomaly>().size() == 0){
        return WIN;
    }
    using grand_timer_t = float;
    auto &timer = registry.ctx().get<grand_timer_t>();
    if(timer <= 0){
        return LOSE;
    }
    return ret;
}
