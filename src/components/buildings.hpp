#pragma once

#include "assets/asset_manager.hpp"
#include "components/collisions.hpp"
#include "components/common.hpp"
#include "components/sprite.hpp"
#include "components/velocity.hpp"
#include <imgui.h>
#include <raylib.h>
#include <raymath.h>
#include <tuple>
namespace an {

struct Building {
    Vector2 city_tile;
    Vector2 hide_rect_min;
    Vector2 hide_rect_max;
    TextureAsset asset;

    [[nodiscard]] Vector2 origin() const {
        auto translate = Vector2Multiply(Vector2Subtract(city_tile, Vector2(0.5f, 0.5f)),
                                         Vector2((float)asset.texture.width, (float)asset.texture.height));

        return translate;
    }

    [[nodiscard]] Vector2 rect_min() const {
        auto translate = Vector2Multiply(Vector2Subtract(city_tile, Vector2(0.5f, 0.5f)),
                                         Vector2((float)asset.texture.width, (float)asset.texture.height));

        return Vector2Add(hide_rect_min, translate);
    }

    [[nodiscard]] Vector2 rect_max() const {
        auto translate = Vector2Multiply(Vector2Subtract(city_tile, Vector2(0.5f, 0.5f)),
                                         Vector2((float)asset.texture.width, (float)asset.texture.height));

        return Vector2Add(hide_rect_max, translate);
    }

    void draw_debug(Color color) const {
        auto min = rect_min();
        auto max = rect_max();
        DrawRectangleV(min, Vector2Subtract(max, min), color);
    }

    static constexpr auto name = "Building";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::DragFloat2("City Tile", &city_tile.x);
        ImGui::DragFloat2("Rect Min", &hide_rect_min.x);
        ImGui::DragFloat2("Rect Max", &hide_rect_max.x);
    }
};

struct Layer1Building {
    static constexpr auto name = "Layer 1 Building";
    static void inspect() {}
};
struct Layer2Building {
    static constexpr auto name = "Layer 2 Building";
    static void inspect() {}
};

inline Vector2 get_building_min(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return Vector2(187.f, 168.f);
    case TextureEnum::CITY_HOUSES_N2:
        return Vector2(187.f, -132.f);
    case TextureEnum::CITY_HOUSES_NE:
        return Vector2(187.f, 168.f);
    case TextureEnum::CITY_HOUSES_NE2:
        return Vector2(187.f, -132.f);
    case TextureEnum::CITY_HOUSES_NW:
        return Vector2(187.f, 168.f);
    case TextureEnum::CITY_HOUSES_NW2:
        return Vector2(187.f, -132.f);
    default:
        return Vector2(0.f, 0.f);
    }
}

inline Vector2 get_building_max(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return Vector2(187.f + 712.f, 168.f + 425.f);
    case TextureEnum::CITY_HOUSES_N2:
        return Vector2(187.f + 712.f, 52.f + 288.f);
    case TextureEnum::CITY_HOUSES_NE:
        return Vector2(187.f + 712.f, 168.f + 425.f);
    case TextureEnum::CITY_HOUSES_NE2:
        return Vector2(187.f + 712.f, 52.f + 288.f);
    case TextureEnum::CITY_HOUSES_NW:
        return Vector2(187.f + 712.f, 168.f + 425.f);
    case TextureEnum::CITY_HOUSES_NW2:
        return Vector2(187.f + 712.f, 52.f + 288.f);
    default:
        return Vector2(0.f, 0.f);
    }
}

inline Vector2 get_coll_min(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return Vector2(191.f, 400.f);
    case TextureEnum::CITY_HOUSES_N2:
        return Vector2(191.f, 96.f);
    case TextureEnum::CITY_HOUSES_NE:
        return Vector2(191.f, 400.f);
    case TextureEnum::CITY_HOUSES_NE2:
        return Vector2(191.f, 96.f);
    case TextureEnum::CITY_HOUSES_NW:
        return Vector2(544.f, 400.f);
    case TextureEnum::CITY_HOUSES_NW2:
        return Vector2(544.f, 96.f);
    default:
        return Vector2(0.f, 0.f);
    }
}

inline Vector2 get_coll_max(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return Vector2(191.f + 706.f, 400.f + 278.f);
    case TextureEnum::CITY_HOUSES_N2:
        return Vector2(191.f + 706.f, 96.f + 278.f);
    case TextureEnum::CITY_HOUSES_NE:
        return Vector2(191.f + 352.f, 400.f + 278.f);
    case TextureEnum::CITY_HOUSES_NE2:
        return Vector2(191.f + 352.f, 96.f + 278.f);
    case TextureEnum::CITY_HOUSES_NW:
        return Vector2(544.f + 352.f, 400.f + 278.f);
    case TextureEnum::CITY_HOUSES_NW2:
        return Vector2(544.f + 352.f, 96.f + 278.f);
    default:
        return Vector2(0.f, 0.f);
    }
}

inline bool has_2coll(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return false;
    case TextureEnum::CITY_HOUSES_N2:
        return false;
    case TextureEnum::CITY_HOUSES_NE:
        return true;
    case TextureEnum::CITY_HOUSES_NE2:
        return true;
    case TextureEnum::CITY_HOUSES_NW:
        return true;
    case TextureEnum::CITY_HOUSES_NW2:
        return true;
    default:
        return false;
    }
}

inline Vector2 get_2coll_min(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return Vector2(0.f, 0.f);
    case TextureEnum::CITY_HOUSES_N2:
        return Vector2(0.f, 0.f);
    case TextureEnum::CITY_HOUSES_NE:
        return Vector2(592.f, 400.f);
    case TextureEnum::CITY_HOUSES_NE2:
        return Vector2(592.f, 96.f);
    case TextureEnum::CITY_HOUSES_NW:
        return Vector2(0.f, 400.f);
    case TextureEnum::CITY_HOUSES_NW2:
        return Vector2(0.f, 96.f);
    default:
        return Vector2(0.f, 0.f);
    }
}

inline Vector2 get_2coll_max(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return Vector2(0.f, 0.f);
    case TextureEnum::CITY_HOUSES_N2:
        return Vector2(0.f, 0.f);
    case TextureEnum::CITY_HOUSES_NE:
        return Vector2(592.f + 496.f, 400.f + 278.f);
    case TextureEnum::CITY_HOUSES_NE2:
        return Vector2(592.f + 496.f, 96.f + 278.f);
    case TextureEnum::CITY_HOUSES_NW:
        return Vector2(0.f + 496.f, 400.f + 278.f);
    case TextureEnum::CITY_HOUSES_NW2:
        return Vector2(0.f + 496.f, 96.f + 278.f);
    default:
        return Vector2(0.f, 0.f);
    }
}

inline float get_building_offset_y(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return 0.f;
    case TextureEnum::CITY_HOUSES_N2:
        return 555.f - 371.f;
    case TextureEnum::CITY_HOUSES_NE:
        return 0.f;
    case TextureEnum::CITY_HOUSES_NE2:
        return 555.f - 371.f;
    case TextureEnum::CITY_HOUSES_NW:
        return 0.f;
    case TextureEnum::CITY_HOUSES_NW2:
        return 555.f - 371.f;
    default:
        return 0.f;
    }
}

inline float get_building_offset_offset_y(TextureEnum id) {
    switch (id) {
    case TextureEnum::CITY_HOUSES_N:
        return 671.f;
    case TextureEnum::CITY_HOUSES_N2:
        return 555.f;
    case TextureEnum::CITY_HOUSES_NE:
        return 671.f;
    case TextureEnum::CITY_HOUSES_NE2:
        return 555.f;
    case TextureEnum::CITY_HOUSES_NW:
        return 671.f;
    case TextureEnum::CITY_HOUSES_NW2:
        return 555.f;
    default:
        return 0.f;
    }
}

inline void make_building_l1(entt::registry &registry, Vector2 tile, TextureEnum id) {
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    auto entity = registry.create();
    auto building = registry.emplace<Building>(entity, tile, get_building_min(id), get_building_max(id), texture);
    registry.emplace<Layer1Building>(entity);
    emplace_sprite(registry, entity, texture);
    auto &building_sprite = std::get<Sprite>(registry.get<Drawable>(entity).sprite);

    auto origin = building.origin();
    auto offset_y = get_building_offset_y(id);
    auto offset_offset_y = get_building_offset_offset_y(id);

    building_sprite.offset = Vector2(0.f, offset_offset_y / texture.texture.height);

    auto collision = registry.create();
    emplace<GlobalTransform>(registry, collision);
    const auto maxx = Vector2Add(get_coll_max(id), {0.f, 10.f});
    emplace<StaticBody>(registry, collision, get_coll_min(id), Vector2Subtract(maxx, get_coll_min(id)));
    auto &local = registry.get<LocalTransform>(collision);
    local.transform.position = origin;

    if (has_2coll(id)) {
        auto collision = registry.create();
        emplace<GlobalTransform>(registry, collision);
        emplace<StaticBody>(registry, collision, get_2coll_min(id),
                            Vector2Subtract(get_2coll_max(id), get_2coll_min(id)));
        auto &local = registry.get<LocalTransform>(collision);
        local.transform.position = origin;
    }

    auto &tr = registry.get<LocalTransform>(entity);
    tr.transform.position = Vector2(origin.x, origin.y - offset_y + offset_offset_y);
}

inline void make_building_l2(entt::registry &registry, Vector2 tile, TextureEnum id) {
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    auto entity = registry.create();
    auto building = registry.emplace<Building>(entity, tile, get_building_min(id), get_building_max(id), texture);
    registry.emplace<Layer2Building>(entity);
    emplace_sprite(registry, entity, texture);
    auto &building_sprite = std::get<Sprite>(registry.get<Drawable>(entity).sprite);

    auto origin = building.origin();
    auto offset_y = get_building_offset_y(id);
    auto offset_offset_y = get_building_offset_offset_y(id);

    building_sprite.offset = Vector2(0.f, offset_offset_y / texture.texture.height);

    auto collision = registry.create();
    emplace<GlobalTransform>(registry, collision);
    emplace<StaticBody>(registry, collision, get_coll_min(id), Vector2Subtract(get_coll_max(id), get_coll_min(id)));
    auto &local = registry.get<LocalTransform>(collision);
    local.transform.position = origin;

    if (has_2coll(id)) {
        auto collision = registry.create();
        emplace<GlobalTransform>(registry, collision);
        emplace<StaticBody>(registry, collision, get_2coll_min(id),
                            Vector2Subtract(get_2coll_max(id), get_2coll_min(id)));
        auto &local = registry.get<LocalTransform>(collision);
        local.transform.position = origin;
    }

    auto &tr = registry.get<LocalTransform>(entity);
    tr.transform.position = Vector2(origin.x, origin.y - offset_y + offset_offset_y);
}

inline void hide_buildings_if_needed(entt::registry &registry, entt::entity player) {
    bool layer_1_hidden = false;
    bool layer_2_hidden = false;

    auto player_pos = registry.get<GlobalTransform>(player).transform.position;
    auto view = registry.view<Building>();

    for (auto &&[entity, building] : view.each()) {
        auto min = building.rect_min();
        auto max = building.rect_max();

        if (min.x <= player_pos.x && player_pos.x <= max.x && min.y <= player_pos.y && player_pos.y <= max.y) {
            if (registry.any_of<Layer1Building>(entity)) {
                layer_1_hidden = true;
            }
            if (registry.any_of<Layer2Building>(entity)) {
                layer_2_hidden = true;
            }
        }
    }

    auto l1_view = registry.view<Layer1Building, Drawable>();
    for (auto &&[entity, drawable] : l1_view.each()) {
        auto tint = ColorAlpha(WHITE, layer_1_hidden ? 0.4f : 1.0f);
        auto &sprite = std::get<Sprite>(drawable.sprite);
        sprite.tint = tint;
    }

    auto l2_view = registry.view<Layer2Building, Drawable>();
    for (auto &&[entity, drawable] : l2_view.each()) {
        auto tint = ColorAlpha(WHITE, layer_2_hidden ? 0.4f : 1.0f);
        auto &sprite = std::get<Sprite>(drawable.sprite);
        sprite.tint = tint;
    }
}

inline void debug_buildings(entt::registry &registry) {
    auto color = ColorAlpha(ORANGE, 0.3f);
    auto view = registry.view<Building>();
    for (auto &&[entity, building] : view.each()) {
        building.draw_debug(color);
    }
}

} // namespace an
