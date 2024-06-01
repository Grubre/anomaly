#include <entt.hpp>
#include <raylib.h>
#include <imgui.h>
namespace an{
struct Corpse {
    static constexpr auto name = "Corpse";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Text("Corpse");
    }
};
auto make_corpse(entt::registry &registry,Vector2 pos) -> entt::entity;

}