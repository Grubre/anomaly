#pragma once
#include <cstdint>
#include <entt.hpp>
#include <imgui.h>
#include <string>
#include "assets/asset_manager.hpp"
#include "components/collisions.hpp"
#include "fstream"
namespace an {
enum class PropType : uint8_t { 
    TREE1, TREE2, 
    ROCK1, ROCK2, 
    BUSH, 
    BIN, 
    LAMP, 
    BENCH, 
    CNT 
};
const char *get_prop_name(const PropType type);
PropType get_prop_type(const std::string &name);
TextureEnum get_prop_texture(const PropType type);
StaticBody get_prop_collision(const PropType type);

struct Prop {
    PropType type;
    bool update;
    static constexpr auto name = "Prop";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
};

template <> void emplace<Prop, PropType>(entt::registry &registry, entt::entity entity, const PropType &type);
template <> void emplace<Prop, Prop>(entt::registry &registry, entt::entity entity, const Prop &prop);
void update_props(entt::registry &registry);
auto spawn_prop(entt::registry &registry) -> entt::entity;
void save_props(entt::registry &registry);
std::ifstream get_ifstream(const char *filename);
void load_props(entt::registry &registry, std::ifstream strm);
[[nodiscard]] auto get_random_prop() -> Prop;
} // namespace an
