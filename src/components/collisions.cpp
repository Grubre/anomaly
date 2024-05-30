#include "collisions.hpp"

auto an::static_vs_character_resolve_vector(an::StaticBody s, an::CharacterBody c) -> std::optional<Vector2> {
    auto px = std::clamp(c.pos.x, s.pos.x, s.pos.x + s.size.x);
    auto py = std::clamp(c.pos.y, s.pos.y, s.pos.y + s.size.y);

    auto diff = Vector2Subtract(c.pos, Vector2(px, py));
    auto is_intersecting = Vector2LengthSqr(diff) <= c.radius * c.radius;

    if (!is_intersecting) {
        return {};
    }

    auto dist = Vector2Length(diff);
    auto norm_vec = Vector2Normalize(diff);
    auto resolve = Vector2Scale(norm_vec, c.radius - dist);

    return resolve;
}

auto an::character_vs_character_resolve_vector(an::CharacterBody a, an::CharacterBody b) -> std::optional<Vector2> {
    auto diff = Vector2Subtract(a.pos, b.pos);
    auto is_intersecting = Vector2LengthSqr(diff) <= (a.radius + b.radius) * (a.radius + b.radius);

    if (!is_intersecting) {
        return {};
    }

    auto dist = Vector2Length(diff);
    auto norm_vec = Vector2Normalize(diff);
    auto resolve = Vector2Scale(norm_vec, a.radius + b.radius - dist);

    return resolve;
}

void an::static_vs_character_collision_system(entt::registry &registry) {
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
                auto &local = registry.get<LocalTransform>(c);
                local.transform.position = Vector2Add(local.transform.position, vector);
            }
        }
    }
}

void an::character_vs_character_collision_system(entt::registry &registry) {
    auto char_view = registry.view<CharacterBody, GlobalTransform>();

    for (auto &&[a, a_body, a_tr] : char_view.each()) {
        auto a_copy = a_body;
        a_copy.pos = Vector2Add(a_copy.pos, a_tr.transform.position);
        a_copy.radius = a_copy.radius * (a_tr.transform.scale.x + a_tr.transform.scale.y) * 0.5f;

        for (auto &&[b, b_body, b_tr] : char_view.each()) {
            auto b_copy = b_body;
            b_copy.pos = Vector2Add(b_copy.pos, b_tr.transform.position);
            b_copy.radius = b_copy.radius * (b_tr.transform.scale.x + b_tr.transform.scale.y) * 0.5f;

            auto resolve = character_vs_character_resolve_vector(a_copy, b_copy);

            if (resolve) {
                auto vector = resolve.value();
                auto a_move = Vector2Scale(vector, 0.5f);
                auto b_move = Vector2Negate(a_move);

                auto &local_a = registry.get<LocalTransform>(a);
                local_a.transform.position = Vector2Add(local_a.transform.position, a_move);

                auto &local_b = registry.get<LocalTransform>(b);
                local_b.transform.position = Vector2Add(local_b.transform.position, b_move);
            }
        }
    }
}

void an::debug_draw_bodies(entt::registry &registry) {
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
