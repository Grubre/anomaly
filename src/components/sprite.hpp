#pragma once
#include <cstdint>
#include <entt.hpp>
#include <raylib.h>
#include <imgui.h>
#include <raymath.h>
#include <variant>
#include "velocity.hpp"
#include "../assets/asset_manager.hpp"
#include "common.hpp"

namespace an {

struct Visible {};

struct ShaderComponent {
    Shader shader;
};

struct Sprite {
    TextureAsset asset;
    uint16_t sprite_id = 0;
    Vector2 offset = Vector2(0.5f, 0.5f);

    void draw(const Transform &tr) const {
        auto src_rect = asset.rect(sprite_id);

        auto width = src_rect.width * tr.scale.x;
        auto height = src_rect.height * tr.scale.y;

        DrawTexturePro(asset.texture, src_rect, Rectangle{tr.position.x, tr.position.y, width, height},
                       Vector2Multiply(offset, Vector2(width, height)), RAD2DEG * tr.rotation, WHITE);
    }

    static constexpr auto name = "Sprite";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Text("Sprite");
        ImGui::DragFloat2("Offset", &offset.x, 1.0f);
        ImGui::DragScalar("Sprite id", ImGuiDataType_U16, &sprite_id, 1.0f);
    }
};

struct CharacterSprite {
    TextureAsset base;
    TextureAsset hair;
    TextureAsset shirt;
    TextureAsset pants;

    Color hair_color = GREEN;
    Color shirt_color = BLUE;
    Color pants_color = RED;

    Vector2 hair_offset = Vector2(0.5f, 0.1f);
    Vector2 shirt_offset = Vector2(0.5f, 0.5f);
    Vector2 pants_offset = Vector2(0.5f, 0.7f);
    Vector2 base_offset = Vector2(0.5f, 0.5f);

    uint16_t sprite_id = 0;

    void draw_component(const Transform &tr, Vector2 offset, TextureAsset asset, Color color) const {
        auto src_rect = asset.rect(sprite_id);

        auto width = src_rect.width * tr.scale.x;
        auto height = src_rect.height * tr.scale.y;

        DrawTexturePro(asset.texture, src_rect, Rectangle{tr.position.x, tr.position.y, width, height},
                       Vector2Multiply(offset, Vector2(width, height)), RAD2DEG * tr.rotation, color);
    }

    void draw(const Transform &tr) const {
        draw_component(tr, base_offset, base, WHITE);
        draw_component(tr, shirt_offset, shirt, shirt_color);
        draw_component(tr, pants_offset, pants, pants_color);
        draw_component(tr, hair_offset, hair, hair_color);
    }

    static constexpr auto name = "Character Sprite";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Text("Character Sprite");
        ImGui::DragFloat2("Hair Offset", &hair_offset.x, 1.0f);
        ImGui::DragScalarN("Hair Color", ImGuiDataType_U8, &hair_color.r, 4);
        ImGui::DragFloat2("Shirt Offset", &shirt_offset.x, 1.0f);
        ImGui::DragScalarN("Shirt Color", ImGuiDataType_U8, &shirt_color.r, 4);
        ImGui::DragFloat2("Pants Offset", &pants_offset.x, 1.0f);
        ImGui::DragScalarN("Pants Color", ImGuiDataType_U8, &pants_color.r, 4);
        ImGui::DragScalar("Sprite id", ImGuiDataType_U16, &sprite_id, 1.0f);
        ImGui::DragFloat2("Base Offset", &base_offset.x, 1.0f);
    }
};

struct Drawable {
    std::variant<Sprite, CharacterSprite> sprite;
};

inline void emplace_sprite(entt::registry &registry, entt::entity entity, const TextureEnum &id) {
    emplace<GlobalTransform>(registry, entity);
    emplace<Visible>(registry, entity);
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    an::safe_emplace<Drawable>(registry, entity, Sprite{texture});
}

inline void emplace_character_sprite(entt::registry &registry, entt::entity entity, const TextureEnum &base_id,
                                     const TextureEnum &hair_id, const TextureEnum &shirt_id,
                                     const TextureEnum &pants_id) {
    emplace<GlobalTransform>(registry, entity);
    emplace<Visible>(registry, entity);
    auto texture = registry.ctx().get<AssetManager>().get_texture(base_id);
    auto texture_hair = registry.ctx().get<AssetManager>().get_texture(hair_id);
    auto texture_shirt = registry.ctx().get<AssetManager>().get_texture(shirt_id);
    auto texture_pants = registry.ctx().get<AssetManager>().get_texture(pants_id);
    an::safe_emplace<Drawable>(registry, entity, CharacterSprite{texture, texture_hair, texture_shirt, texture_pants });
}

template <> inline void emplace<Sprite>(entt::registry &registry, entt::entity entity, const TextureEnum &id) {
    emplace_sprite(registry, entity, id);
}

template<>
inline void emplace<CharacterSprite>(entt::registry &registry, entt::entity entity, const TextureEnum &base_id,
                                     const TextureEnum &hair_id, const TextureEnum &shirt_id,
                                     const TextureEnum &pants_id) {
    emplace_character_sprite(registry, entity, base_id, hair_id, shirt_id, pants_id);
}

inline void render_drawables(entt::registry &registry) {
    auto drawables = registry.view<Drawable, GlobalTransform, Visible>();
    for (auto &&[entity, drawable, transform] : drawables.each()) {
        if (registry.all_of<ShaderComponent>(entity)) {
            const auto &shader = registry.get<ShaderComponent>(entity).shader;
            BeginShaderMode(shader);
        }

        std::visit([&](auto &drawable) { drawable.draw(transform.transform); }, drawable.sprite);

        if (registry.all_of<ShaderComponent>(entity)) {
            EndShaderMode();
        }
    }
}
} // namespace an
