#include "marker.hpp"
#include "characters.hpp"
#include "velocity.hpp"
#include "raylib.h"
#include "raymath.h"
#include "gui/inspect_window.hpp"
namespace an {

void Mark::inspect(entt::registry &registry, entt::entity entity) {
    static constexpr int minimum = 0;
    static constexpr int maximum = 1;
    ImGui::SliderScalar("Mark Type", ImGuiDataType_U8, &type, &minimum, &maximum);
}
void remove_interrupt(entt::registry &registry, entt::entity entity) {
    registry.remove<Interrupted>(entity);
}
void add_interrupt(entt::registry &registry, entt::entity entity) {
    registry.emplace_or_replace<Interrupted>(entity);
}
void check_nearby_npc(entt::registry &registry, entt::entity entity) {
    auto view = registry.view<Character, GlobalTransform>();
    auto co = 0;
    for (auto &&[npc, transform] : view.each()) {
        auto dis=Vector2Distance(registry.get<GlobalTransform>(entity).transform.position, transform.transform.position);
        if ( dis<70){
            co++;
            if((co==1||dis>30)&&registry.all_of<ShowUI>(entity)){
                registry.erase<ShowUI>(entity);
                continue;
            }
            registry.emplace_or_replace<ShowUI>(entity);
            add_interrupt(registry,npc);
        }
    }
}
void check_interrupted_npc(entt::registry &registry, entt::entity entity) {
    auto view = registry.view<Interrupted, GlobalTransform>();
    for (auto &&[npc, transform] : view.each()) {
        if (Vector2Distance(registry.get<GlobalTransform>(entity).transform.position, transform.transform.position) >
            80) {
            registry.remove<ShowUI>(entity);
            registry.remove<Interrupted>(npc);
        }
    }
}
void update_marker_system(entt::registry &registry, entt::entity entity) { check_interrupted_npc(registry, entity); }
} // namespace an