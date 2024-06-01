#pragma once
#include <cstdint>
#include <raylib.h>
#include <array>
#include <entt.hpp>

namespace an {

enum class TextureEnum {
    PLAYER_TEXTURE,
    MAIN_CHAR,
    BASE_CHARACTER,
    CHARACTER_SHIRT_1,
    CHARACTER_SHIRT_2,
    CHARACTER_SHIRT_4,
    CHARACTER_PANTS_1,
    CHARACTER_PANTS_2,
    CHARACTER_PANTS_4,
    CHARACTER_HAIR_1,
    CHARACTER_HAIR_2,
    CHARACTER_HAIR_4,
    TEST_TILE,
    TREE1,
    TREE2,
    ROCK1,
    ROCK2,
    BUSH,
    BIN,
    LAMP,
    BENCH,
    DRUNK_PARTICLE,
    STINKY_PARTICLE,
    B_SUS,
    B_LEGIT,
    B_BACK,
    UI_BACKGROUND,
    CITY_TILE_N,
    CITY_TILE_NE,
    CITY_TILE_NW,
    CITY_TILE_W,
    CITY_TILE_SQUARE,
    CITY_TILE_E,
    CITY_HOUSES_N,
    CITY_HOUSES_N2,
    CITY_HOUSES_NE,
    CITY_HOUSES_NE2,
    CITY_HOUSES_NW,
    CITY_HOUSES_NW2,
    MARKER,
    BULLET,
    STICK,
    SEESAW,
    HAMMER,
    NAIL,
    BOBER,
    DIALOG_CLOUD,
    CORPSE,
    DROPS,
    GAME_OVER,
    WIN,
    CNT
};
enum class SoundEnum { WIN = 0, CNT };

struct TextureAsset {
    Texture2D texture;
    uint16_t cell_size_x;
    uint16_t cell_size_y;

    [[nodiscard]] Rectangle rect(uint16_t sprite_id, bool flip_h, bool flip_v) const {
        auto columns_in_texture = texture.width / cell_size_x;

        auto row = sprite_id / columns_in_texture;
        auto column = sprite_id % columns_in_texture;

        int width = cell_size_x;
        if (flip_h) {
            width = -width;
        }

        int height = cell_size_y;
        if (flip_v) {
            height = -height;
        }

        return Rectangle{.x = static_cast<float>(column * cell_size_x),
                         .y = static_cast<float>(row * cell_size_y),
                         .width = static_cast<float>(width),
                         .height = static_cast<float>(height)};
    }

    void unload() {
        UnloadTexture(texture);
        texture.id = 0;
    }
};

class AssetManager {
  public:
    void register_texture(const Image &image, TextureEnum id);
    void register_texture(const Image &image, TextureEnum id, uint16_t cell_size_x, uint16_t cell_size_y);
    auto get_texture(TextureEnum id) -> TextureAsset;
    auto get_texture_ptr(TextureEnum id) -> TextureAsset*;
    void register_sound(const Sound &sound, const SoundEnum id);
    auto get_sound(const SoundEnum id) -> Sound;

  private:
    std::array<TextureAsset, static_cast<size_t>(TextureEnum::CNT)> textures;
    std::array<Sound, static_cast<size_t>(SoundEnum::CNT)> sounds;
};

/*
create_bullet() {
    emplace<Sprite>(registry, bullet, bullet_sprite);
}
*/

} // namespace an
