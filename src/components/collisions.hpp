#pragma once

#include "components/velocity.hpp"
#include <algorithm>
#include <optional>
#include <raylib.h>
#include <entt.hpp>
#include <raymath.h>

namespace an {

/// Static body - a rectangle with top left corner in pos and size of size
struct StaticBody {
    Vector2 pos;
    Vector2 size;

    static constexpr auto name = "Static Body";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("Position", &pos.x, 1);
        ImGui::DragFloat2("Size", &size.x, 1);
    }
};

/// Movable character body - a circle with center in pos and radius of radius
struct CharacterBody {
    Vector2 pos;
    float radius;

    static constexpr auto name = "Character Body";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("Position", &pos.x, 1);
        ImGui::DragFloat("Radius", &radius, 1);
    }
};

auto static_vs_character_resolve_vector(an::StaticBody s, an::CharacterBody c) -> std::optional<Vector2>;
auto character_vs_character_resolve_vector(CharacterBody a, CharacterBody b) -> std::optional<Vector2>;
void static_vs_character_collision_system(entt::registry &registry);
void character_vs_character_collision_system(entt::registry &registry);
void debug_draw_bodies(entt::registry &registry);

} // namespace an
