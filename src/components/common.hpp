#pragma once
#include <entt.hpp>

namespace bh {

template <typename T, typename... Args>
void safe_emplace(entt::registry &registry, entt::entity entity, const Args &...args) {
    if (registry.all_of<T>(entity)) {
        return;
    }
    registry.emplace<T>(entity, args...);
}

template <typename T, typename... Args>
void emplace(entt::registry &registry, entt::entity entity, const Args &...args) {
    safe_emplace<T>(registry, entity, args...);
}

} // namespace bh
