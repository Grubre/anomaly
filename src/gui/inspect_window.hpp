#pragma once
#include <imgui.h>
#include <entt.hpp>
#include "assets/asset_manager.hpp"
#include "components/marker.hpp"
namespace an {
void draw_inspect_dialog(entt::registry &registry, entt::entity entity);
void update_ui(entt::registry &registry, entt::entity entity);
} // namespace an