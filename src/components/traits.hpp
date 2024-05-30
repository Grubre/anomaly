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

    static constexpr auto name = "Avoid Trait";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragScalar("Prop", ImGuiDataType_U8, &prop, 1.0f);
        ImGui::DragFloat("Radius", &radius, 1);
    }
};

inline void avoid_trait_system(entt::registry& registry) {
    auto char_view = registry.view<AvoidTraitComponent, GlobalTransform>();
    auto prop_view = registry.view<prop, GlobalTransform>();

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
                registry.emplace<EscapeCharState>(char_entity, 5.f, escape_dir, 50.f);
            }
        }
    }
}

inline void trait_systems(entt::registry& registry) {
    avoid_trait_system(registry);
} 

}