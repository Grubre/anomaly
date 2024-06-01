#pragma once
#include <entt.hpp>
#include <raylib.h>
#include <imgui.h>
namespace an{
enum WinCondition {
    WIN,
    LOSE,
    NONE
};
struct Corpse {
    static constexpr auto name = "Corpse";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Text("Corpse");
    }
};
auto make_corpse(entt::registry &registry,Vector2 pos) -> entt::entity;
WinCondition check_win_condition(entt::registry &registry);
}