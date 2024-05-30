#pragma once
#include <cstdint>
#include <entt.hpp>
#include <imgui.h>
#include <string>
#include <sstream>
#include "assets/asset_manager.hpp"
#include "common.hpp"
#include "components/collisions.hpp"
#include "velocity.hpp"
#include "sprite.hpp"
#include "gui/inspector.hpp"
#include "fstream"
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
inline StaticBody get_prop_collision(const PropType type) {
    switch (type) {
    case PropType::TREE:
        return StaticBody { { -15.f, -15.f }, { 30.f, 30.f } };
    case PropType::ROCK:
        return StaticBody { { -15.f, -15.f }, { 30.f, 30.f } };
    case PropType::LAMP:
        return StaticBody { { -15.f, -15.f }, { 30.f, 30.f } };
    case PropType::BENCH:
        return StaticBody { { -15.f, -15.f }, { 30.f, 30.f } };
    default:
        return StaticBody { { -15.f, -15.f }, { 30.f, 30.f } };
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
    emplace<StaticBody>(registry, entity, get_prop_collision(type));
}
template <> inline void emplace<Prop, Prop>(entt::registry &registry, entt::entity entity, const Prop& prop) {
    registry.emplace<Prop>(entity, prop.type);
    emplace<Sprite>(registry, entity, get_prop_texture(prop.type));
    emplace<GlobalTransform>(registry, entity);
    emplace<Visible>(registry, entity);
    safe_emplace<DebugName>(registry, entity, get_prop_name(prop.type));
    emplace<StaticBody>(registry, entity, get_prop_collision(prop.type));
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
inline void save_props(entt::registry &registry){
auto view = registry.view<Prop,GlobalTransform>();
    std::ofstream MyFile("props.dat");
    for (auto &&[entity, prop,transform] : view.each()) {
        MyFile << static_cast<int>(prop.type) << " " << transform.transform.position.x << " " << transform.transform.position.y << std::endl;
    }
}
inline std::ifstream get_ifstream(const char * filename){
    return std::ifstream(filename);
}
inline void load_props(entt::registry& registry, std::ifstream strm){
    std::string line;
    while (getline(strm,line)){
        auto entity = registry.create();
        std::stringstream ss(line);
        std::string tmp;
        getline(ss,tmp,' ');
        emplace<Prop,PropType>(registry,entity,static_cast<PropType>(std::stoi(tmp)));
        auto &transform = registry.get<LocalTransform>(entity);
        getline(ss,tmp,' ');
        transform.transform.position.x = std::stof(tmp);
        getline(ss,tmp,' ');
        transform.transform.position.y = std::stof(tmp);
    }
}
} // namespace an
