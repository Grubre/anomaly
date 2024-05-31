#include "marker.hpp"
#include "characters.hpp"
#include "velocity.hpp"
#include "raylib.h"
#include "raymath.h"
#include "gui/inspect_window.hpp"
#include "relations.hpp"
namespace an {

void Marked::inspect() { ImGui::Text("Entity marked"); }
void remove_interrupt(entt::registry &registry, entt::entity entity) { registry.remove<Interrupted>(entity); }
void add_interrupt(entt::registry &registry, entt::entity entity) { registry.emplace_or_replace<Interrupted>(entity); }
void check_nearby_npc(entt::registry &registry, entt::entity entity) {
    auto view = registry.view<Character, GlobalTransform>();
    auto co = 0;
    for (auto &&[npc, transform] : view.each()) {
        auto dis =
            Vector2Distance(registry.get<GlobalTransform>(entity).transform.position, transform.transform.position);
        if (dis < 70) {
            co++;
            if ((co == 1 || dis > 30) && registry.all_of<ShowUI>(entity)) {
                registry.erase<ShowUI>(entity);
                continue;
            }
            registry.emplace_or_replace<ShowUI>(entity);
            add_interrupt(registry, npc);
        }
    }
}
void check_marks(entt::registry &registry) {
    auto view = registry.view<Marker, Parented>();
    for (auto &&[entity, parent] : view.each()) {
        if (!registry.all_of<Marked>(parent.parent)) {
            registry.destroy(entity);
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
void update_marker_system(entt::registry &registry, entt::entity entity) {
    check_interrupted_npc(registry, entity);
    check_marks(registry);
}

template <> void emplace<Marker>(entt::registry &registry, entt::entity entity) {
    emplace<Sprite>(registry, entity, TextureEnum::MARKER);
    registry.emplace<Marker>(entity);
}
void mark_entity(entt::registry &registry, entt::entity entity) {
    if (registry.all_of<Marked>(entity)) {
        return;
    }
    registry.emplace<Marked>(entity);
    auto mark_e = registry.create();
    emplace<Parented>(registry, mark_e, entity);
    emplace<Marker>(registry, mark_e);
    auto &sprite = registry.get<Drawable>(mark_e);
    std::get<Sprite>(sprite.sprite).offset.y = 1.8f;
    // transform.transform.scale = Vector2Scale(transform.transform.scale, 0.5f);
}
void Marker::inspect() { ImGui::Text("Marker entity"); }
} // namespace an