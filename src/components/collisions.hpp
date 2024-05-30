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
};

/// Movable character body - a circle with center in pos and radius of radius
struct CharacterBody {
    Vector2 pos;
    float radius;
};

std::optional<Vector2> static_vs_character_resolve_vector(StaticBody s, CharacterBody c) {
    auto px = std::clamp(c.pos.x, s.pos.x, s.pos.x + s.size.x);
    auto py = std::clamp(c.pos.y, s.pos.y, s.pos.y + s.size.y);

    auto diff = Vector2Subtract(c.pos, Vector2(px,py));
    auto is_intersecting = Vector2LengthSqr(diff) <= c.radius * c.radius;

    if (!is_intersecting) {
        return {};
    }

    auto norm_vec = Vector2Normalize(diff);
    auto dist = Vector2Length(norm_vec);
    auto resolve = Vector2Scale(norm_vec, c.radius - dist);

    return resolve;
}

void static_vs_character_collision_system(entt::registry &registry) {
    auto static_view = registry.view<StaticBody, GlobalTransform>();
    auto char_view = registry.view<CharacterBody, GlobalTransform>();

    for (auto &&[c, c_body, c_tr] : char_view.each()) {
        auto c_copy = c_body;
        c_copy.pos = Vector2Add(c_copy.pos, c_tr.transform.position);
        c_copy.radius = c_copy.radius * (c_tr.transform.scale.x + c_tr.transform.scale.y) * 0.5f;

        for (auto &&[s, s_body, s_tr] : static_view.each()) {
            auto s_copy = s_body;

            s_copy.pos = Vector2Add(s_copy.pos, s_tr.transform.position);
            s_copy.size = Vector2Multiply(s_copy.size, s_tr.transform.scale);

            auto resolve = static_vs_character_resolve_vector(s_copy, c_copy);

            if (resolve) {
                auto vector = resolve.value();
                auto local = registry.get<LocalTransform>(c);
                local.transform.position = Vector2Add(local.transform.position, vector);
            }
        }
    }
}

void debug_draw_bodies(entt::registry &registry) {
    auto char_view = registry.view<CharacterBody, GlobalTransform>();
    auto char_color = ColorAlpha(GREEN, 0.5f);

    for (auto &&[c, c_body, c_tr] : char_view.each()) {
        auto c_copy = c_body;
        c_copy.pos = Vector2Add(c_copy.pos, c_tr.transform.position);
        c_copy.radius = c_copy.radius * (c_tr.transform.scale.x + c_tr.transform.scale.y) * 0.5f;

        DrawCircleV(c_copy.pos, c_copy.radius, char_color);
    }

    auto static_view = registry.view<StaticBody, GlobalTransform>();
    auto static_color = ColorAlpha(BLUE, 0.5f);

    for (auto &&[s, s_body, s_tr] : static_view.each()) {
        auto s_copy = s_body;

        s_copy.pos = Vector2Add(s_copy.pos, s_tr.transform.position);
        s_copy.size = Vector2Multiply(s_copy.size, s_tr.transform.scale);

        DrawRectangleV(s_copy.pos, s_copy.size, static_color);
    }
}

}
