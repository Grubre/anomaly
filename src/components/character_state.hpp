#pragma once

#include "components/collisions.hpp"
#include "components/velocity.hpp"
#include <algorithm>
#include <fmt/format.h>
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

struct FollowPathState {
    static constexpr auto name = "Follow Path State";

    float speed;
    std::vector<Vector2> points;
    std::vector<float> wait_times;
    std::uint32_t current_point{0u};
    float time_elapsed{0.f};

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat("Speed", &speed, 1);
        ImGui::Text("Number of points: %zu", points.size());
        for (std::size_t i = 0; i < points.size(); ++i) {
            ImGui::Text("Point %zu", i);
            ImGui::SameLine();

            std::string label_point = "Point##" + std::to_string(i);
            ImGui::DragFloat2(label_point.c_str(), &points[i].x, 1);

            std::string label_wait_time = "Wait Time##" + std::to_string(i);
            ImGui::DragFloat(label_wait_time.c_str(), &wait_times[i], 1);
        }
        if (ImGui::Button("Add Point")) {
            points.push_back({0.f, 0.f});
            wait_times.push_back(0.f);
        }
    }
};

struct RandomWalkState {
    static constexpr auto name = "Random Walk State";

    float speed{};
    Vector2 target{};
    float wait_time{};
    float time_elapsed{0.f};

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat("Speed", &speed, 1);
        ImGui::DragFloat2("Vector", &target.x, 1);
        ImGui::DragFloat("Wait Time", &wait_time, 1);
    }
};

inline void random_walk_state_system(entt::registry &registry) {
    constexpr float epsilon = 40.f;
    auto view = registry.view<RandomWalkState, LocalTransform>();

    for (auto &&[entity, state, transform] : view.each()) {
        const auto target = state.target;
        const auto dir = Vector2Normalize(Vector2Subtract(target, transform.transform.position));
        const auto delta = Vector2Scale(dir, state.speed * GetFrameTime());

        if (Vector2Distance(state.target, transform.transform.position) < epsilon) {
            state.time_elapsed += GetFrameTime();
            if (state.time_elapsed >= state.wait_time) {
                state.wait_time = get_uniform_float() * 3.f + 2.f;
                state.speed = std::clamp(get_uniform_float() * 120.f + 50.f ,50.f, 120.f);
                state.time_elapsed = 0.f;

                Vector2 candidate_target =
                    Vector2Add(state.target, Vector2{get_random_float(-300.f, 300.f), get_random_float(-100.f, 100.f)});
                while (is_in_any_static(registry, candidate_target)) {
                    candidate_target = Vector2Add(
                        state.target, Vector2{get_random_float(-300.f, 300.f), get_random_float(-100.f, 100.f)});
                }
                state.target = candidate_target;
            }
            continue;
        }

        DrawCircleV(state.target, 5, ColorAlpha(BLACK, 0.5f));
        transform.transform.position = Vector2Add(transform.transform.position, delta);
        state.time_elapsed += GetFrameTime();
    }
}

inline void follow_path_state_system(entt::registry &registry) {
    constexpr float epsilon = 1.f;
    auto view = registry.view<FollowPathState, LocalTransform>();

    for (auto &&[entity, state, transform] : view.each()) {
        const auto target = state.points[state.current_point];
        const auto dir = Vector2Normalize(Vector2Subtract(target, transform.transform.position));
        const auto delta = Vector2Scale(dir, state.speed * GetFrameTime());

        if (Vector2Distance(transform.transform.position, target) < epsilon) {
            state.time_elapsed += GetFrameTime();
            if (state.wait_times[state.current_point] - state.time_elapsed <= 0.001f) {
                state.time_elapsed = 0.f;
                state.current_point = (state.current_point + 1) % state.points.size();
            }
            continue;
        }

        transform.transform.position = Vector2Add(transform.transform.position, delta);
    }
}

inline void follow_entity_character_state_system(entt::registry &registry) {
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

inline void escape_character_state_system(entt::registry &registry) {
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

inline void character_states_systems(entt::registry &registry) {
    follow_entity_character_state_system(registry);
    escape_character_state_system(registry);
    follow_path_state_system(registry);
    random_walk_state_system(registry);
}

} // namespace an
