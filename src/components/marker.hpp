#pragma once
#include <entt.hpp>
#include <imgui.h>
namespace an {
struct Interrupted {
    static constexpr auto name = "Interrupted";
    static void inspect() { ImGui::Text("NPC Interrupted"); }
};
struct ShowUI {
    static constexpr auto name = "ShowUI";
    static void inspect() { ImGui::Text("UI Visible"); }
};
struct Marked {
    static constexpr auto name = "Marked";
    static void inspect();
};
struct Marker {
    static constexpr auto name = "Marker";
    static void inspect();
};
void mark_entity(entt::registry &registry, entt::entity entity);
void check_nearby_npc(entt::registry &registry, entt::entity entity);
void update_marker_system(entt::registry &registry, entt::entity entity);
void remove_interrupt(entt::registry &registry, entt::entity entity);
void add_interrupt(entt::registry &registry, entt::entity entity);
} // namespace an