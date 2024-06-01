#pragma once

#include "characters.hpp"
#include "components/collisions.hpp"
#include "components/common.hpp"
#include "components/marker.hpp"
#include "components/velocity.hpp"
#include "components/walk_area.hpp"
#include "components/player.hpp"
#include "gui/inspector.hpp"
#include <algorithm>
#include <cmath>
#include <fmt/format.h>
#include <imgui.h>
#include <numbers>
#include <raylib.h>
#include <entt.hpp>
#include <raymath.h>
#include <stack>
#include <variant>

namespace an {

struct FollowEntityState {
    entt::entity entity;
    float time_left;
    float speed;
    float last_speed_update_time;

    static constexpr auto name = "Follow Entity Character State";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};

struct Aggresive {
    static constexpr auto name = "Aggresive";
    static void inspect() {}
};

struct EscapeState {
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

template <typename... States> struct PossibleCharState {
    std::variant<States...> variant{};

    void apply_self(entt::registry &registry, entt::entity entity) {
        std::visit([&]<typename State>(State &t) { registry.emplace_or_replace<State>(entity, t); }, variant);
    }

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        std::visit(
            [&](auto &t) {
                ImGui::Text(t.name);
                t.inspect(registry, entity);
            },
            variant);
    }
};

template <typename... States> struct CharacterStateMachineT {
    std::vector<PossibleCharState<States...>> stack;

    void pop_and_apply(entt::registry &registry, entt::entity entity) {
        if (!stack.empty()) {
            auto top = stack.back();
            top.apply_self(registry, entity);
            stack.pop_back();
        }
    }

    void save_current_state(entt::registry &registry, entt::entity entity) {
        (
            [&]() {
                if (registry.all_of<States>(entity)) {
                    States state = registry.get<States>(entity);
                    stack.emplace_back(state);
                    registry.remove<States>(entity);
                }
            }(),
            ...);
    }

    static constexpr auto name = "Character State Machine";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        if (ImGui::Button("Pop and apply")) {
            pop_and_apply(registry, entity);
        }
        ImGui::SameLine(0.f, 20.f);
        if (ImGui::Button("Save current")) {
            save_current_state(registry, entity);
        }
        for (auto &s : stack) {
            s.inspect(registry, entity);
        }
    }
};

using CharacterStateMachine = CharacterStateMachineT<RandomWalkState, FollowPathState, FollowEntityState, EscapeState>;

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

struct WalkingState {
    static constexpr auto name = "Walking state";
    Direction direction{};

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        // up, down, left, right list selection
        static const char *directions[] = {"UP", "DOWN", "LEFT", "RIGHT"};
        int current_direction = static_cast<int>(direction);
        ImGui::Combo("Direction", &current_direction, directions, IM_ARRAYSIZE(directions));
        direction = static_cast<Direction>(current_direction);
    }
};

struct IdleState {
    static constexpr auto name = "Idle state";
    Direction direction{};

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        // up, down, left, right list selection
        static const char *directions[] = {"UP", "DOWN", "LEFT", "RIGHT"};
        int current_direction = static_cast<int>(direction);
        ImGui::Combo("Direction", &current_direction, directions, IM_ARRAYSIZE(directions));
        direction = static_cast<Direction>(current_direction);
    }
};

inline void set_move_state_system(entt::registry &registry) {
    auto walking_view = registry.view<WalkingState, const Velocity>();

    for (auto &&[entity, walking_state, velocity] : walking_view.each()) {
        if (velocity.x == 0 && velocity.y == 0) {
            registry.remove<WalkingState>(entity);
            an::emplace<IdleState>(registry, entity, walking_state.direction);
            continue;
        }
        const auto atan2 = std::atan2(velocity.y, velocity.x);
        const auto pi = std::numbers::pi_v<float>;
        if (-pi / 4.f <= atan2 && atan2 <= pi / 4.f) {
            walking_state.direction = Direction::RIGHT;
        } else if (pi / 4.f < atan2 && atan2 <= 3 * pi / 4.f) {
            walking_state.direction = Direction::DOWN;
        } else if (-3.f * pi / 4.f <= atan2 && atan2 < -pi / 4.f) {
            walking_state.direction = Direction::UP;
        } else {
            walking_state.direction = Direction::LEFT;
        }
    }

    auto idle_view = registry.view<IdleState, const Velocity>();

    for (auto &&[entity, idle, velocity] : idle_view.each()) {
        if (velocity.x != 0 || velocity.y != 0) {
            an::emplace<WalkingState>(registry, entity);
            registry.remove<IdleState>(entity);
        }
    }
}

inline void init_interrupted_state_and_menu_bridge(entt::registry &registry) {
    registry.on_construct<Interrupted>().connect<[&](entt::registry &registry, entt::entity entity) {
        auto &state_machine = registry.get<CharacterStateMachine>(entity);
        state_machine.save_current_state(registry, entity);
    }>();

    registry.on_destroy<Interrupted>().connect<[&](entt::registry &registry, entt::entity entity) {
        auto &state_machine = registry.get<CharacterStateMachine>(entity);
        state_machine.pop_and_apply(registry, entity);
    }>();
}

inline void update_character_animations(entt::registry &registry) {
    auto animations = registry.view<Animation, WalkingState, Drawable>();

    auto get_frame_offset = [](Direction direction) {
        switch (direction) {
        case Direction::DOWN:
            return 0u;
        case Direction::UP:
            return 4u;
        case Direction::RIGHT:
            return 8u;
        case Direction::LEFT:
            return 12u;
        }
        return 0u;
    };

    for (auto &&[entity, animation, state, drawable] : animations.each()) {
        animation.time_acc += GetFrameTime();
        const auto frame_offset = get_frame_offset(state.direction);
        if (animation.time_acc >= animation.time_between_frames) {
            animation.time_acc = 0.f;
            animation.current_frame = frame_offset + (animation.current_frame + 1) % animation.len;
        }

        std::visit([&](auto &drawable) { drawable.sprite_id = animation.current_frame; }, drawable.sprite);
    }

    auto idle = registry.view<IdleState, Drawable>();

    for (auto &&[entity, idle, drawable] : idle.each()) {
        std::visit([&](auto &drawable) { drawable.sprite_id = get_frame_offset(idle.direction); }, drawable.sprite);
    }
}

void follow_player_if_bullet(entt::registry &registry, entt::entity character, entt::entity bullet);

void random_walk_state_system(entt::registry &registry);

void follow_path_state_system(entt::registry &registry);

void follow_entity_character_state_system(entt::registry &registry);

void escape_character_state_system(entt::registry &registry);

void character_states_systems(entt::registry &registry);

} // namespace an
