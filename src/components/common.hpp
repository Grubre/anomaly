#pragma once
#include <entt.hpp>
#include <raylib.h>

namespace an {

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

[[nodiscard]] inline auto get_random_float(float min, float max) -> float {
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}

} // namespace an
