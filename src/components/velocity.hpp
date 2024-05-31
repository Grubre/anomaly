#pragma once

#include <imgui.h>
#include <raylib.h>
#include <raymath.h>
#include <entt.hpp>
#include "common.hpp"

namespace an {

struct Transform {
    static constexpr auto name = "Transform";
    Vector2 position;
    float rotation;
    Vector2 scale = Vector2(1.0f, 1.0f);

    /// Aplikuje najpierw swój transform, a dopiero potem other
    [[nodiscard]] Transform combine(const Transform &other) const {
        auto transformed_local = transform_point(other.position);
        return Transform{transformed_local, rotation + other.rotation, Vector2Multiply(scale, other.scale)};
    }

    [[nodiscard]] Vector2 transform_point(const Vector2 &point) const {
        auto scaled = Vector2Multiply(point, scale);
        auto rotated = Vector2Rotate(scaled, rotation);
        auto translated = Vector2Add(rotated, position);

        return translated;
    }

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("Transform", &position.x, 1.0f);
        ImGui::DragFloat("Rotation", &rotation, 0.05f);
        ImGui::DragFloat2("Scale", &scale.x, 0.5f);
    }
};

struct LocalTransform {
    Transform transform;

    static constexpr auto name = "Local Transform";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        transform.inspect(registry, entity);
    }
};

struct GlobalTransform {
    Transform transform;

    static constexpr auto name = "Global Transform";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        transform.inspect(registry, entity);
    }
};

template <> inline void emplace<LocalTransform>(entt::registry &registry, entt::entity entity) {
    safe_emplace<GlobalTransform>(registry, entity);
    safe_emplace<LocalTransform>(registry, entity);
}

template <> inline void emplace<GlobalTransform>(entt::registry &registry, entt::entity entity) {
    safe_emplace<GlobalTransform>(registry, entity);
    safe_emplace<LocalTransform>(registry, entity);
}
struct Velocity {
    static constexpr auto name = "Velocity";
    float x = 0;
    float y = 0;
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Value("X", x);
        ImGui::Value("Y", y);
    }
    Velocity &operator=(const Vector2 &other) {
        this->x = other.x;
        this->y = other.y;
        return *this;
    }
    explicit operator Vector2() const { return {x, y}; }
};

template <> inline void emplace<Velocity>(entt::registry &registry, entt::entity entity) {
    emplace<LocalTransform>(registry, entity);
    safe_emplace<Velocity>(registry, entity);
}

inline void move_things(entt::registry &registry) {
    auto view = registry.view<Velocity, LocalTransform>();
    for (auto &&[entity, vel, transform] : view.each()) {
        transform.transform.position.x += vel.x * GetFrameTime();
        transform.transform.position.y += vel.y * GetFrameTime();
    }
}

} // namespace an
