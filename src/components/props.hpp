#pragma once
#include <cstdint>
#include <entt.hpp>
#include <imgui.h>
#include <string>
#include "assets/asset_manager.hpp"
#include "common.hpp"
#include "velocity.hpp"
#include "sprite.hpp"
#include "gui/inspector.hpp"
namespace an {
enum class PropType : uint8_t { TREE, ROCK, LAMP, BENCH, CNT };
inline const char *get_prop_name(const PropType type) {
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
inline PropType get_prop_type(const std::string &name) {
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
inline TextureEnum get_prop_texture(const PropType type) {
    switch (type) {
    case PropType::TREE:
        return TextureEnum::TREE;
    case PropType::ROCK:
        return TextureEnum::ROCK;
    case PropType::LAMP:
        return TextureEnum::LAMP;
    case PropType::BENCH:
        return TextureEnum::BENCH;
    default:
        return TextureEnum::CNT;
    }
}
struct Prop {
    PropType type;
    bool update;
    static constexpr auto name = "Prop";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        const size_t minimum = 0;
        const size_t maximum = static_cast<size_t>(PropType::CNT)-1;
        ImGui::Text("%s", get_prop_name(type));
        ImGui::SliderScalar("Type", ImGuiDataType_U8, &type, &minimum, &maximum);
        ImGui::Checkbox("Update", &update);
    }
};
template <> inline void emplace<Prop, PropType>(entt::registry &registry, entt::entity entity, const PropType &type) {
    auto prop = registry.emplace<Prop>(entity, type);
    emplace<Sprite>(registry, entity, get_prop_texture(type));
    emplace<GlobalTransform>(registry, entity);
    emplace<Visible>(registry, entity);
    emplace<DebugName>(registry, entity, get_prop_name(type));
}
template <> inline void emplace<Prop, Prop>(entt::registry &registry, entt::entity entity, const Prop& prop) {
    registry.emplace<Prop>(entity, prop.type);
    emplace<Sprite>(registry, entity, get_prop_texture(prop.type));
    emplace<GlobalTransform>(registry, entity);
    emplace<Visible>(registry, entity);
    safe_emplace<DebugName>(registry, entity, get_prop_name(prop.type));
}
inline void update_props(entt::registry &registry) {
    auto view = registry.view<Prop, Sprite>();
    for (auto &&[entity, prop, sprite] : view.each()) {
        if (prop.update) {
            registry.erase<Sprite>(entity);
            registry.erase<DebugName>(entity);
            emplace<Sprite>(registry, entity, get_prop_texture(prop.type));
            emplace<DebugName>(registry, entity, get_prop_name(prop.type));
            prop.update = false;
        }
    }
}

} // namespace an
