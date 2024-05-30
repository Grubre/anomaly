#pragma once

#include "components/velocity.hpp"
#include <imgui.h>
#include <raylib.h>
#include <entt.hpp>
#include <raymath.h>

namespace an {

struct FollowEntityCharState {
    entt::entity entity;
    float time_left;    
    float speed;

    static constexpr auto name = "Follow Entity Character State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat("Time Left", &time_left, 1);
        ImGui::DragFloat("Speed", &speed, 1);
        ImGui::Text("Following : %d", (int)this->entity);
    }
};

struct EscapeCharState {
    float time_left;
    Vector2 direction;
    float speed;

    static constexpr auto name = "Escape Character State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat("Time Left", &time_left, 1);
        ImGui::DragFloat2("Direction", &direction.x, 1);
        ImGui::DragFloat("Speed", &speed, 1);
    }
};

inline void follow_entity_character_state_system(entt::registry& registry) {
    auto view = registry.view<FollowEntityCharState, GlobalTransform, LocalTransform>();

    for (auto &&[entity, state, global, local] : view.each()) {
        state.time_left -= GetFrameTime();

        if (state.time_left <= 0.f) {
            registry.remove<FollowEntityCharState>(entity);
            continue;
        }

        auto target_position = registry.get<GlobalTransform>(state.entity).transform.position;

        auto dir = Vector2Normalize(Vector2Subtract(target_position, global.transform.position));
        auto delta = Vector2Scale(dir, state.speed * GetFrameTime());
        local.transform.position = Vector2Add(local.transform.position, delta);
    } 
}

inline void escape_character_state_system(entt::registry& registry) {
    auto view = registry.view<EscapeCharState, LocalTransform>();

    for (auto &&[entity, state, transform] : view.each()) {
        state.time_left -= GetFrameTime();

        if (state.time_left <= 0.f) {
            registry.remove<EscapeCharState>(entity);
            continue;
        }

        auto delta = Vector2Scale(state.direction, state.speed * GetFrameTime());
        transform.transform.position = Vector2Add(transform.transform.position, delta);
    }
}

inline void character_states_systems(entt::registry& registry) {
    follow_entity_character_state_system(registry);
    escape_character_state_system(registry);
}

}