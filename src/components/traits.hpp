#pragma once

#include "components/character_state.hpp"
#include "components/props.hpp"
#include "components/velocity.hpp"
#include <raylib.h>
#include <raymath.h>
namespace an {

struct AvoidTraitComponent {
    PropType prop;
    float radius;
    float escape_speed;

    static constexpr auto name = "Avoid Trait";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragScalar("Prop", ImGuiDataType_U8, &prop, 1.0f);
        ImGui::DragFloat("Radius", &radius, 1);
        ImGui::DragFloat("Escape Speed", &escape_speed, 1);
    }
};

struct ShakeTraitComponent {
    PropType prop;
    float radius;
    float shake_constant;

    static constexpr auto name = "Shake Trait";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragScalar("Prop", ImGuiDataType_U8, &prop, 1.0f);
        ImGui::DragFloat("Radius", &radius, 1);
        ImGui::DragFloat("Shake Constant", &shake_constant, 1);
    }
};

inline void avoid_trait_system(entt::registry& registry) {
    auto char_view = registry.view<AvoidTraitComponent, GlobalTransform>();
    auto prop_view = registry.view<Prop, GlobalTransform>();

    for (auto &&[char_entity, avoid_trait, char_tr] : char_view.each()) {
        for (auto &&[prop_entity, prop, prop_tr] : prop_view.each()) {
            if (prop.type != avoid_trait.prop) {
                continue;
            }

            auto delta = Vector2Subtract(char_tr.transform.position, prop_tr.transform.position);
            auto delta_len = Vector2LengthSqr(delta);
            if (delta_len <= avoid_trait.radius * avoid_trait.radius) {
                auto escape_dir = Vector2Normalize(delta);
                registry.remove<FollowEntityCharState>(char_entity);
                safe_emplace<EscapeCharState>(registry, char_entity, 5.f, escape_dir, avoid_trait.escape_speed);
            }
        }
    }
}

inline void shake_trait_system(entt::registry& registry) {
    auto char_view = registry.view<ShakeTraitComponent, GlobalTransform>();
    auto prop_view = registry.view<Prop, GlobalTransform>();

    for (auto &&[char_entity, shake_trait, char_tr] : char_view.each()) {
        for (auto &&[prop_entity, prop, prop_tr] : prop_view.each()) {
            if (prop.type != shake_trait.prop) {
                continue;
            }

            auto delta = Vector2Subtract(char_tr.transform.position, prop_tr.transform.position);
            auto delta_len = Vector2LengthSqr(delta);
            if (delta_len <= shake_trait.radius * shake_trait.radius) {
                auto shake_coeff = shake_trait.shake_constant / delta_len;

                float x_r = ((float) rand() / RAND_MAX) * 2.f - 1.f;
                float y_r = ((float) rand() / RAND_MAX) * 2.f - 1.f;

                auto shake_vec = Vector2Scale(Vector2Normalize(Vector2(x_r,y_r)), shake_coeff);

                auto &local = registry.get<LocalTransform>(char_entity);

                local.transform.position = Vector2Add(char_tr.transform.position, shake_vec);
            }
        }
    }
}

inline void trait_systems(entt::registry& registry) {
    avoid_trait_system(registry);
    shake_trait_system(registry);
} 

inline void debug_shake_trait_system(entt::registry& registry) {
    auto trait_view = registry.view<ShakeTraitComponent, GlobalTransform>();
    auto trait_color = ColorAlpha(RED, 0.2f);

    for (auto &&[entity, trait, tr] : trait_view.each()) {
        DrawCircleV(tr.transform.position, trait.radius, trait_color);
    }
}

inline void debug_avoid_trait_system(entt::registry& registry) {
    auto trait_view = registry.view<AvoidTraitComponent, GlobalTransform>();
    auto trait_color = ColorAlpha(RED, 0.2f);

    for (auto &&[entity, trait, tr] : trait_view.each()) {
        DrawCircleV(tr.transform.position, trait.radius, trait_color);
    }
}

inline void debug_trait_systems(entt::registry& registry) {
    debug_shake_trait_system(registry);
    debug_avoid_trait_system(registry);
}

}