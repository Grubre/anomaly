#pragma once

#include "character_generator.hpp"
#include "components/common.hpp"
#include <entt.hpp>

namespace an {

struct Character {};

inline entt::entity make_character(entt::registry &registry, const CharacterTraits& traits) {
    auto entity = registry.create();
    an::emplace<an::Character>(registry, entity);
    return entity;
}

inline void turn_into_anomaly(entt::registry &registry, entt::entity character, const AnomalyTraits& traits) {

}

}
