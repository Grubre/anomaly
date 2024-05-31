#pragma once

#include "characters.hpp"
#include "components/collisions.hpp"
#include "components/common.hpp"
#include "components/velocity.hpp"
#include "components/walk_area.hpp"
#include "components/player.hpp"
#include <algorithm>
#include <cmath>
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
    float last_speed_update_time;

    static constexpr auto name = "Follow Entity Character State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};

struct EscapeCharState {
    float time_left;
    Vector2 direction;
    float speed;

    static constexpr auto name = "Escape Character State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};

struct FollowPathState {
    static constexpr auto name = "Follow Path State";

    float speed;
    std::vector<Vector2> points;
    std::vector<float> wait_times;
    std::uint32_t current_point{0u};
    float time_elapsed{0.f};

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};

struct RandomWalkState {
    static constexpr auto name = "Random Walk State";

    float speed{};
    Vector2 target{};
    float wait_time{};
    const WalkArea *walk_area;

    float time_elapsed{0.f};

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};

void remove_character_state(entt::registry &registry, entt::entity entity);

void follow_player_if_bullet(entt::registry &registry, entt::entity character, entt::entity bullet);

void random_walk_state_system(entt::registry &registry);

void follow_path_state_system(entt::registry &registry);

void follow_entity_character_state_system(entt::registry &registry);

void escape_character_state_system(entt::registry &registry);

void character_states_systems(entt::registry &registry);

} // namespace an
