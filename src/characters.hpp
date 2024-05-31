#pragma once

#include "character_generator.hpp"
#include "components/common.hpp"
#include <cstdlib>
#include <entt.hpp>

namespace an {

struct Character {
    static constexpr auto name = "Character";
    static void inspect() { ImGui::Text("EPIC NPC"); }
};

entt::entity make_character(entt::registry &registry, const CharacterTraits &traits);

void turn_into_anomaly(entt::registry &registry, entt::entity character, const AnomalyTraits &traits);

} // namespace an
