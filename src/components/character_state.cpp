#include "character_state.hpp"
#include "marker.hpp"
#include <algorithm>
#include <raylib.h>
namespace an {
void FollowEntityState::inspect(entt::registry &registry, entt::entity entity) {
    ImGui::DragFloat("Time Left", &time_left, 1);
    ImGui::DragFloat("Speed", &speed, 1);
    ImGui::Text("Following : %d", (int)this->entity);
}

void EscapeState::inspect(entt::registry &registry, entt::entity entity) {
    ImGui::DragFloat("Time Left", &time_left, 1);
    ImGui::DragFloat2("Direction", &direction.x, 1);
    ImGui::DragFloat("Speed", &speed, 1);
}

void FollowPathState::inspect(entt::registry &registry, entt::entity entity) {
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

void RandomWalkState::inspect(entt::registry &registry, entt::entity entity) {
    ImGui::DragFloat("Speed", &speed, 1);
    ImGui::DragFloat2("Vector", &target.x, 1);
    ImGui::DragFloat("Wait Time", &wait_time, 1);
}

void follow_player_if_bullet(entt::registry &registry, entt::entity character, entt::entity bullet) {
    if (registry.all_of<Character>(character) && registry.all_of<Bullet>(bullet)) {
        if (registry.all_of<FollowEntityState>(character)) {
            auto &state = registry.get<FollowEntityState>(character);
            if (GetTime() >= state.last_speed_update_time + 0.5f) {
                state.speed += 50.f;
                state.time_left += 1.f;
                state.last_speed_update_time = GetTime();
            }
        } else {
            auto &state_machine = registry.get<CharacterStateMachine>(character);
            state_machine.save_current_state(registry, character);

            auto b = registry.get<Bullet>(bullet);
            emplace<FollowEntityState>(registry, character, b.player, 5.f, 100.f, GetTime());
        }
    }
}

void random_walk_state_system(entt::registry &registry) {
    constexpr float epsilon = 40.f;
    auto view = registry.view<RandomWalkState, LocalTransform, Velocity>();

    for (auto &&[entity, state, transform, velocity] : view.each()) {
        const auto target = state.target;
        const auto dir = Vector2Normalize(Vector2Subtract(target, transform.transform.position));
        const auto delta = Vector2Scale(dir, state.speed);

        if (Vector2Distance(state.target, transform.transform.position) < epsilon) {
            state.time_elapsed += GetFrameTime();
            if (state.time_elapsed >= state.wait_time) {
                state.wait_time = get_uniform_float() * 3.f + 2.f;
                state.speed = std::clamp(get_uniform_float() * 120.f + 50.f, 50.f, 120.f);
                state.time_elapsed = 0.f;

                auto [next_target, new_area] = state.walk_area->sample_uniform_neighbourhood_point();
                state.target = next_target;
                state.walk_area = new_area;
            }
            continue;
        }

        state.time_elapsed += GetFrameTime();
        velocity = delta;
    }
}

void follow_path_state_system(entt::registry &registry) {
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

void follow_entity_character_state_system(entt::registry &registry) {
    auto view = registry.view<FollowEntityState, GlobalTransform, LocalTransform, CharacterStateMachine, Velocity>();

    for (auto &&[entity, state, global, local, state_machine, velocity] : view.each()) {
        state.time_left -= GetFrameTime();

        if (state.time_left <= 0.f) {
            registry.remove<FollowEntityState>(entity);
            state_machine.pop_and_apply(registry, entity);
            continue;
        }

        auto target_position = registry.get<GlobalTransform>(state.entity).transform.position;

        auto dir = Vector2Normalize(Vector2Subtract(target_position, global.transform.position));
        auto delta = Vector2Scale(dir, state.speed);
        velocity = delta;
    }
}

void escape_character_state_system(entt::registry &registry) {
    auto view = registry.view<EscapeState, LocalTransform, CharacterStateMachine, Velocity>();

    for (auto &&[entity, state, transform, state_machine, velocity] : view.each()) {
        state.time_left -= GetFrameTime();

        if (state.time_left <= 0.f) {
            registry.remove<EscapeState>(entity);
            state_machine.pop_and_apply(registry, entity);
            continue;
        }

        auto delta = Vector2Scale(state.direction, state.speed);
        velocity = delta;
    }
}

void character_states_systems(entt::registry &registry) {
    follow_entity_character_state_system(registry);
    escape_character_state_system(registry);
    follow_path_state_system(registry);
    random_walk_state_system(registry);
}
} // namespace an
