#pragma once
#include <cstdint>
#include <cstdio>
#include <entt.hpp>
#include <raylib.h>
#include <imgui.h>
#include <raymath.h>
#include <variant>
#include "assets/assets_loader.hpp"
#include "velocity.hpp"
#include "../assets/asset_manager.hpp"
#include "common.hpp"

namespace an {

struct TintShader {
    Shader shader;
    int loc;
};

inline void init_tint_shader(entt::registry &registry) {
    const Shader tint_shader = load_asset(LoadShader, "shaders/base.vs", "shaders/base.fs");

    const int tint_loc = GetShaderLocation(tint_shader, "tint");

    registry.ctx().emplace<TintShader>(tint_shader, tint_loc);
}

struct Visible {};

struct ShaderComponent {
    Shader shader;
};

struct EdgeDetectionShader {
    Shader shader;
    int resolution_loc;
};

struct Selected {};

inline void init_edge_detection_shader(entt::registry &registry) {
    const Shader shader = an::load_asset(LoadShader, "shaders/base.vs", "shaders/scuffed_edge_detection.fs");
    const auto resolution = GetShaderLocation(shader, "resolution");

    registry.ctx().emplace<EdgeDetectionShader>(shader, resolution);
}

struct Sprite {
    TextureAsset asset;
    uint16_t sprite_id = 0;
    Vector2 offset = Vector2(0.5f, 0.5f);
    bool flip_h = false;
    bool flip_v = false;

    void draw(const Transform &tr, const EdgeDetectionShader *shader) const {
        auto src_rect = asset.rect(sprite_id, flip_h, flip_v);

        auto width = std::abs(src_rect.width) * tr.scale.x;
        auto height = std::abs(src_rect.height) * tr.scale.y;
        auto size_vec = Vector2{width, height};

        if (shader != nullptr) {
            BeginShaderMode(shader->shader);
            SetShaderValue(shader->shader, shader->resolution_loc, &size_vec, SHADER_UNIFORM_VEC2);
        }

        DrawTexturePro(asset.texture, src_rect, Rectangle{tr.position.x, tr.position.y, width, height},
                       Vector2Multiply(offset, Vector2(width, height)), RAD2DEG * tr.rotation, WHITE);

        if (shader != nullptr) {
            EndShaderMode();
        }
    }

    static constexpr auto name = "Sprite";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Text("Sprite");
        ImGui::DragFloat2("Offset", &offset.x, 1.0f);
        ImGui::Checkbox("Flip H", &flip_h);
        ImGui::SameLine();
        ImGui::Checkbox("Flip V", &flip_v);
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

    Vector2 hair_offset = Vector2(0.5f, 0.5f);
    Vector2 shirt_offset = Vector2(0.5f, 0.5f);
    Vector2 pants_offset = Vector2(0.5f, 0.5f);
    Vector2 base_offset = Vector2(0.5f, 0.5f);

    uint16_t sprite_id = 0;

    bool flip_h = false;
    bool flip_v = false;

    void draw_component(const Transform &tr, Vector2 offset, TextureAsset asset, Color color,
                        TintShader tint_shader) const {

        auto norm_color =
            Vector4((float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f, (float)color.a / 255.f);

        SetShaderValue(tint_shader.shader, tint_shader.loc, &norm_color, SHADER_UNIFORM_VEC4);

        auto src_rect = asset.rect(sprite_id, flip_h, flip_v);

        auto width = std::abs(src_rect.width) * tr.scale.x;
        auto height = std::abs(src_rect.height) * tr.scale.y;

        BeginShaderMode(tint_shader.shader);

        DrawTexturePro(asset.texture, src_rect, Rectangle{tr.position.x, tr.position.y, width, height},
                       Vector2Multiply(offset, Vector2(width, height)), RAD2DEG * tr.rotation, WHITE);

        EndShaderMode();
    }

    void draw(const Transform &tr, const TintShader &tint_shader) const {
        draw_component(tr, base_offset, base, WHITE, tint_shader);
        draw_component(tr, shirt_offset, shirt, shirt_color, tint_shader);
        draw_component(tr, pants_offset, pants, pants_color, tint_shader);
        draw_component(tr, hair_offset, hair, hair_color, tint_shader);
    }

    static constexpr auto name = "Character Sprite";
    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        ImGui::Text("Character Sprite");
        ImGui::DragFloat2("Hair Offset", &hair_offset.x, 0.01f);
        ImGui::DragScalarN("Hair Color", ImGuiDataType_U8, &hair_color.r, 4);
        ImGui::DragFloat2("Shirt Offset", &shirt_offset.x, 0.01f);
        ImGui::DragScalarN("Shirt Color", ImGuiDataType_U8, &shirt_color.r, 4);
        ImGui::DragFloat2("Pants Offset", &pants_offset.x, 0.01f);
        ImGui::DragScalarN("Pants Color", ImGuiDataType_U8, &pants_color.r, 4);
        ImGui::Checkbox("Flip H", &flip_h);
        ImGui::SameLine();
        ImGui::Checkbox("Flip V", &flip_v);
        ImGui::DragScalar("Sprite id", ImGuiDataType_U16, &sprite_id, 1.0f);
        ImGui::DragFloat2("Base Offset", &base_offset.x, 0.01f);
    }
};

struct Drawable {
    std::variant<Sprite, CharacterSprite> sprite;

    static constexpr auto name = "Drawable";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        std::visit([&](auto &drawable) { drawable.inspect(registry, entity); }, sprite);
    }
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
    an::safe_emplace<Drawable>(registry, entity, CharacterSprite{texture, texture_hair, texture_shirt, texture_pants});
}

template <> inline void emplace<Sprite>(entt::registry &registry, entt::entity entity, const TextureEnum &id) {
    emplace_sprite(registry, entity, id);
}

template <>
inline void emplace<CharacterSprite>(entt::registry &registry, entt::entity entity, const TextureEnum &base_id,
                                     const TextureEnum &hair_id, const TextureEnum &shirt_id,
                                     const TextureEnum &pants_id) {
    emplace_character_sprite(registry, entity, base_id, hair_id, shirt_id, pants_id);
}

inline void render_drawables(entt::registry &registry) {
    auto drawables = registry.view<Drawable, GlobalTransform, Visible>();
    drawables.use<Visible>();

    auto tint_shader = registry.ctx().get<TintShader>();

    for (auto &&[entity, drawable, transform] : drawables.each()) {
        bool has_shader = registry.all_of<ShaderComponent>(entity);
        if (has_shader) {
            const auto &shader = registry.get<ShaderComponent>(entity).shader;
            BeginShaderMode(shader);
        }

        bool is_selected = registry.all_of<Selected>(entity);

        std::visit(entt::overloaded{[&](CharacterSprite &sprite) { sprite.draw(transform.transform, tint_shader); },
                                    [&](Sprite &sprite) {
                                        sprite.draw(transform.transform,
                                                    is_selected ? &registry.ctx().get<EdgeDetectionShader>() : nullptr);
                                    }},
                   drawable.sprite);

        if (has_shader) {
            EndShaderMode();
        }
    }
}

inline void y_sort(entt::registry &registry) {
    registry.sort<Visible>([&](const entt::entity lhs, const entt::entity rhs) {
        auto y_lhs = registry.get<GlobalTransform>(lhs).transform.position.y;
        auto y_rhs = registry.get<GlobalTransform>(rhs).transform.position.y;
        return y_lhs < y_rhs;
    });
}
} // namespace an
