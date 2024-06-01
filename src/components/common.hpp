#pragma once
#include <entt.hpp>
#include <random>
#include <raylib.h>
#include <imgui.h>

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

[[nodiscard]] inline auto get_uniform_float() -> float {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    return dis(gen);
}

[[nodiscard]] inline auto get_random_float(float min, float max) -> float {
    return get_uniform_float() * (max - min) + min;
}
struct Anomaly {
    static constexpr auto name = "Anomaly";
    static void inspect() { ImGui::Text("Is anomaly"); }
};
} // namespace an
