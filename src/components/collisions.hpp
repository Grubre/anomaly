#pragma once

#include "components/velocity.hpp"
#include <algorithm>
#include <optional>
#include <raylib.h>
#include <entt.hpp>
#include <raymath.h>
#include <vector>

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
    float mass = 1.f;

    static constexpr auto name = "Character Body";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("Position", &pos.x, 1);
        ImGui::DragFloat("Radius", &radius, 1);
        ImGui::DragFloat("Mass", &mass, 0.1f);
    }
};

struct NarrowPair {
    entt::entity first;
    entt::entity second;
};

enum class SASPointKind {
    BEGIN,
    END,
};

struct SortAndSweepPoint {
    float point;
    entt::entity entity;
    SASPointKind kind;
};

struct CollisionController {
    std::vector<NarrowPair> narrow_queue;
    std::vector<SortAndSweepPoint> sas_list;
    std::unordered_set<entt::entity> active_sas;
};

inline void init_collision_controller(entt::registry &registry) {
    registry.ctx().emplace<CollisionController>();
    registry.on_construct<CharacterBody>().connect<[](entt::registry& registry, entt::entity entity) {
        auto &controller = registry.ctx().get<CollisionController>();
        controller.sas_list.emplace_back(0.f, entity, SASPointKind::BEGIN);
        controller.sas_list.emplace_back(0.f, entity, SASPointKind::END);
    }>();
}

bool is_in_static(an::StaticBody s, Vector2 point);
auto static_vs_character_resolve_vector(an::StaticBody s, an::CharacterBody c) -> std::optional<Vector2>;
auto character_vs_character_resolve_vector(CharacterBody a, CharacterBody b) -> std::optional<Vector2>;
void static_vs_character_collision_system(entt::registry &registry);
void character_vs_character_collision_system(entt::registry &registry);
bool is_in_any_static(entt::registry &registry, Vector2 point);
void debug_draw_bodies(entt::registry &registry);

} // namespace an
