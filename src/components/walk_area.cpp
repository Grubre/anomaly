#include "walk_area.hpp"
#include "components/collisions.hpp"

[[nodiscard]] auto an::WalkArea::calculate_area() const -> float {
    return std::abs(left_top.x - right_bottom.x) * std::abs(left_top.y - right_bottom.y);
}

[[nodiscard]] auto an::WalkArea::calculate_total_neighbouring_area() const -> float {
    float area = calculate_area();
    for (const auto &neighbour : neighbours) {
        area += neighbour->calculate_area();
    }
    return area;
}

[[nodiscard]] auto an::WalkArea::sample_random_point() const -> Vector2 {
    float x = an::get_random_float(left_top.x, right_bottom.x);
    float y = an::get_random_float(left_top.y, right_bottom.y);

    return Vector2{x, y};
}

[[nodiscard]] auto an::WalkArea::sample_uniform_neighbourhood_point() const -> std::pair<Vector2, const WalkArea *> {
    const auto k = an::get_uniform_float();
    const auto total_area = calculate_total_neighbouring_area();

    auto sum = calculate_area() / total_area;

    if (k < sum) {
        return {sample_random_point(), this};
    }

    for (const auto &neighbour : neighbours) {
        sum += neighbour->calculate_area() / total_area;
        if (k < sum) {
            return {neighbour->sample_random_point(), neighbour};
        }
    }

    return {sample_random_point(), this};
}

void an::connect_walk_areas(an::WalkArea &a, an::WalkArea &b) {
    a.add_neighbour(&b);
    b.add_neighbour(&a);
}

void an::visualize_walk_areas(entt::registry &registry) {
    auto view = registry.view<WalkArea>();
    for (auto &&[entity, walk_area] : view.each()) {
        if (!walk_area.is_visible()) {
            continue;
        }
        DrawRectangleV(walk_area.get_left_top(),
                       Vector2{walk_area.get_right_bottom().x - walk_area.get_left_top().x,
                               walk_area.get_right_bottom().y - walk_area.get_left_top().y},
                       ColorAlpha(ORANGE, 0.3f));
    }
}
