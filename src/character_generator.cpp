#include "character_generator.hpp"
#include <random>

void an::CharacterGenerator::generate_characters(std::uint32_t characters_cnt) {
    generated_characters.reserve(characters_cnt);

    std::mt19937 gen(seed);
    std::uniform_int_distribution<std::size_t> color_dist(0, possible_shirt_colors.size() - 1);

    for (auto i = 0u; i < characters_cnt; i++) {
        CharacterTraits traits{};

        traits.shirt_color = possible_shirt_colors.at(color_dist(gen));
        traits.pants_color = possible_shirt_colors.at(color_dist(gen));
        traits.hair_color = possible_hair_colors.at(color_dist(gen));

        traits.accesories_mask = accesories_mask_t{};

        for (std::size_t i = 0; i < traits.accesories_mask.size(); i++) {
            if (std::bernoulli_distribution{accessory_gen_chance}(gen)) {
                traits.accesories_mask.set(i);
            }
        }

        generated_characters.push_back(traits);
    }
}

auto an::CharacterGenerator::get_original_character_traits() const -> std::span<const CharacterTraits> {
    return generated_characters;
}

[[nodiscard]] auto an::get_config_for_day(std::uint32_t day_number) -> an::DayConfig { return {3, 5, 3, 1}; }
