#include "collisions.hpp"
#include "components/character_state.hpp"
#include "components/common.hpp"
#include "components/velocity.hpp"
#include <cstddef>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>


bool an::is_in_static(an::StaticBody s, Vector2 point) {
    return s.pos.x <= point.x && s.pos.y <= point.y && point.x <= s.pos.x + s.size.x && point.y <= s.pos.y + s.size.y;
}

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
    auto perp = Vector2Scale(Vector2(-resolve.y, resolve.x), 0.1f);
    resolve = Vector2Add(resolve, perp);

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
    auto cc = registry.ctx().get<CollisionController>();

    int i = 0;
    while ((size_t)i < cc.sas_list.size()) {
        auto &sas = cc.sas_list[(size_t)i];

        if (!registry.valid(sas.entity)) {
            std::iter_swap(cc.sas_list.begin() + i, cc.sas_list.end() - 1);
            cc.sas_list.pop_back();
            continue;
        }

        auto tr = registry.get<GlobalTransform>(sas.entity);
        auto body = registry.get<CharacterBody>(sas.entity);

        if (sas.kind == SASPointKind::BEGIN) {
            sas.point = tr.transform.position.x + body.pos.x - body.radius;
        } else {
            sas.point = tr.transform.position.x + body.pos.x + body.radius;
        }

        i += 1;
    }

    std::sort(cc.sas_list.begin(), cc.sas_list.end(), 
        [](auto& a, auto& b) {
            return a.point < b.point;
        }
    );

    for (auto &point : cc.sas_list) {
        if (cc.active_sas.contains(point.entity)) {
            //assert(point.kind == SASPointKind::End);
            cc.active_sas.erase(point.entity);
        } else {
            //assert(point.kind == SASPointKind::Begin);
            
            for (const auto& active : cc.active_sas) {
                cc.narrow_queue.emplace_back(point.entity, active);
            }
            cc.active_sas.insert(point.entity);
        }
    }
    assert(cc.active_sas.empty());

    for (auto &&[a, b] : cc.narrow_queue) {
        auto a_tr = registry.get<GlobalTransform>(a);
        auto a_body = registry.get<CharacterBody>(a);
        a_body.pos = Vector2Add(a_body.pos, a_tr.transform.position);
        a_body.radius = a_body.radius * (a_tr.transform.scale.x + a_tr.transform.scale.y) * 0.5f;

        auto b_tr = registry.get<GlobalTransform>(b);
        auto b_body = registry.get<CharacterBody>(b);
        b_body.pos = Vector2Add(b_body.pos, b_tr.transform.position);
        b_body.radius = b_body.radius * (b_tr.transform.scale.x + b_tr.transform.scale.y) * 0.5f;

        auto resolve = character_vs_character_resolve_vector(a_body, b_body);

        if (resolve) {
            auto vector = resolve.value();
            auto total_mass = a_body.mass + b_body.mass;

            auto a_move = Vector2Scale(vector, b_body.mass / total_mass);
            auto b_move = Vector2Scale(vector, -a_body.mass / total_mass);

            auto &local_a = registry.get<LocalTransform>(a);
            local_a.transform.position = Vector2Add(local_a.transform.position, a_move);

            auto &local_b = registry.get<LocalTransform>(b);
            local_b.transform.position = Vector2Add(local_b.transform.position, b_move);

            follow_player_if_bullet(registry, a, b);
            follow_player_if_bullet(registry, b, a);
        }
    }
}



bool an::is_in_any_static(entt::registry &registry, Vector2 point) {
    auto static_view = registry.view<StaticBody, GlobalTransform>();

    for (auto &&[s, s_body, s_tr] : static_view.each()) {
        if (is_in_static(s_body, point)) {
            return true;
        }
    }

    return false;
}

void an::debug_draw_bodies(entt::registry &registry) {
    auto char_view = registry.view<CharacterBody, GlobalTransform>();
    auto char_color = ColorAlpha(GREEN, 0.2f);

    for (auto &&[c, c_body, c_tr] : char_view.each()) {
        auto c_copy = c_body;
        c_copy.pos = Vector2Add(c_copy.pos, c_tr.transform.position);
        c_copy.radius = c_copy.radius * (c_tr.transform.scale.x + c_tr.transform.scale.y) * 0.5f;

        DrawCircleV(c_copy.pos, c_copy.radius, char_color);
    }

    auto static_view = registry.view<StaticBody, GlobalTransform>();
    auto static_color = ColorAlpha(BLUE, 0.2f);

    for (auto &&[s, s_body, s_tr] : static_view.each()) {
        auto s_copy = s_body;

        s_copy.pos = Vector2Add(s_copy.pos, s_tr.transform.position);
        s_copy.size = Vector2Multiply(s_copy.size, s_tr.transform.scale);

        DrawRectangleV(s_copy.pos, s_copy.size, static_color);
    }
}
