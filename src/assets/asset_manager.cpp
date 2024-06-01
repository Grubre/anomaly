#include "asset_manager.hpp"

void an::AssetManager::register_texture(const Image &image, TextureEnum id) {
    const auto texture = LoadTextureFromImage(image);
    UnloadImage(image);
    textures.at(static_cast<size_t>(id)) = TextureAsset { texture, static_cast<uint16_t>(texture.width), static_cast<uint16_t>(texture.height) };
}
void an::AssetManager::register_sound(const Sound &sound, an::SoundEnum id) {
    sounds.at(static_cast<size_t>(id)) = sound;
}

void an::AssetManager::register_texture(const Image &image, TextureEnum id, uint16_t cell_size_x, uint16_t cell_size_y) {
    const auto texture = LoadTextureFromImage(image);
    UnloadImage(image);
    textures.at(static_cast<size_t>(id)) = TextureAsset { texture, cell_size_x, cell_size_y};
}

auto an::AssetManager::get_texture(TextureEnum id) -> TextureAsset { return textures.at(static_cast<size_t>(id)); }

auto an::AssetManager::get_texture_ptr(TextureEnum id) -> TextureAsset* { 
    return &textures.at(static_cast<size_t>(id)); 
}

auto an::AssetManager::get_sound(SoundEnum id) -> Sound { return sounds.at(static_cast<size_t>(id)); }
