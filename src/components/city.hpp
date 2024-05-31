#include "assets/asset_manager.hpp"
#include "components/common.hpp"
#include "components/sprite.hpp"
#include <raylib.h>
#include <raymath.h>

namespace an {

struct CityTile {
    TextureAsset city_asset;
    Vector2 tile_pos;
};

template <>
void emplace<CityTile>(entt::registry &registry, entt::entity entity, const TextureEnum &id, const Vector2 &tile_pos) {
    auto texture = registry.ctx().get<AssetManager>().get_texture(id);
    an::safe_emplace<CityTile>(registry, entity, texture, tile_pos);
}

void make_city_tile(entt::registry &registry, const TextureEnum &id, const Vector2 &tile_pos) {
    auto entity = registry.create();
    emplace<CityTile>(registry, entity, id, tile_pos);
}

void render_city_tiles(entt::registry &registry) {
    auto view = registry.view<CityTile>();

    for (auto &&[entity, tile] : view.each()) {
        auto vec =
            Vector2Multiply(Vector2Subtract(tile.tile_pos, Vector2(0.5f,0.5f)), Vector2((float)tile.city_asset.texture.width, (float)tile.city_asset.texture.height));
        DrawTextureV(tile.city_asset.texture, vec, WHITE);
    }
}

} // namespace an
