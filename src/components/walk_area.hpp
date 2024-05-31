#pragma once

#include "components/common.hpp"
#include "components/sprite.hpp"
#include <cstdlib>
#include <imgui.h>
#include <raylib.h>

namespace an {

struct WalkArea {
  public:
    static constexpr auto name = "WalkArea";

    WalkArea() = default;

    WalkArea(Vector2 left_top, Vector2 right_bottom) : left_top(left_top), right_bottom(right_bottom) {}
    [[nodiscard]] auto calculate_area() const -> float;
    [[nodiscard]] auto calculate_total_neighbouring_area() const -> float;
    [[nodiscard]] auto sample_random_point() const -> Vector2;
    [[nodiscard]] auto sample_uniform_neighbourhood_point() const -> std::pair<Vector2, const WalkArea *>;
    [[nodiscard]] auto get_left_top() const -> Vector2 { return left_top; }
    [[nodiscard]] auto get_right_bottom() const -> Vector2 { return right_bottom; }
    void add_neighbour(WalkArea *neighbour) { neighbours.push_back(neighbour); }

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        static bool visible = false;
        ImGui::Text("WalkArea");
        ImGui::Text("Number of neighbours: %zu", neighbours.size());
        ImGui::DragFloat2("Left top", &left_top.x, 1.0f);
        ImGui::DragFloat2("Right bottom", &right_bottom.x, 1.0f);
        ImGui::Checkbox("Visible", &visible);

        if (visible) {
            an::emplace<an::Visible>(registry, entity);
        } else {
            registry.remove<an::Visible>(entity);
        }
    }

  private:
    Vector2 left_top{};
    Vector2 right_bottom{};
    std::vector<WalkArea *> neighbours{};
};

void connect_walk_areas(WalkArea &a, WalkArea &b);
void visualize_walk_areas(entt::registry &registry);

} // namespace an
