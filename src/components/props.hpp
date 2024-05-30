#pragma once
#include <cstdint>
#include <entt.hpp>
#include <imgui.h>
#include <string>
#include "velocity.hpp"
#include "sprite.hpp"
#include "gui/inspector.hpp"
namespace an {
enum class PropType : uint8_t { TREE, ROCK, LAMP, BENCH, CNT };
const char *get_prop_name(const PropType type) {
    switch (type) {
    case PropType::TREE:
        return "Tree";
    case PropType::ROCK:
        return "Rock";
    case PropType::LAMP:
        return "Lamp";
    case PropType::BENCH:
        return "Bench";
    default:
        return "None";
    }
}
PropType get_prop_type(const std::string &name) {
    if (name == "Tree") {
        return PropType::TREE;
    } else if (name == "Rock") {
        return PropType::ROCK;
    } else if (name == "Lamp") {
        return PropType::LAMP;
    } else if (name == "Bench") {
        return PropType::BENCH;
    } else {
        return PropType::CNT;
    }
}

struct prop {
    PropType type;
    static constexpr auto name = "Prop";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Text("%s", get_prop_name(type));
        ImGui::DragScalar("Type", ImGuiDataType_U8, &type, 1.0f);
    }
};
template <> inline void emplace<prop, PropType>(entt::registry &registry, entt::entity entity, const PropType &type) {}
[[nodiscard]] entt::entity make_prop(entt::registry &registry, const PropType type) {
    const auto entity = registry.create();
    registry.emplace<prop>(entity, type);
    registry.emplace<GlobalTransform>(entity);
    registry.emplace<LocalTransform>(entity);
    registry.emplace<DebugName>(entity, get_prop_name(type));
    registry.emplace<Visible>(entity);
    return entity;
}
} // namespace an
