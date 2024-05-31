#include "character_generator.hpp"
#include <random>
#include <ranges>
#include <vector>

void an::CharacterGenerator::generate_characters(std::uint32_t characters_cnt) {
    generated_characters.reserve(characters_cnt);

    std::mt19937 gen(seed);
    std::uniform_int_distribution<std::size_t> color_dist(0, possible_shirt_colors.size() - 1);
    std::uniform_int_distribution<std::size_t> hair_dist(0, possible_hair_colors.size() - 1);

    for (auto i = 0u; i < characters_cnt; i++) {
        CharacterTraits traits{};

        traits.shirt_color.color = possible_shirt_colors.at(color_dist(gen));
        traits.pants_color.color = possible_shirt_colors.at(color_dist(gen));
        traits.hair_color.color = possible_hair_colors.at(hair_dist(gen));

        traits.accesories_mask = accesories_mask_t{};

        // for (std::size_t i = 0; i < traits.accesories_mask.size(); i++) {
        //     if (std::bernoulli_distribution{accessory_gen_chance}(gen)) {
        //         traits.accesories_mask.set(i);
        //     }
        // }

        generated_characters.push_back(traits);
    }
}
[[nodiscard]] auto an::probable_trait_name_to_str(const an::ProbableTrait &trait) -> std::string_view {
    return std::visit(entt::overloaded{[](const an::ShirtColor &) { return "ShirtColor"; },
                                       [](const an::PantsColor &) { return "PantsColor"; },
                                       [](const an::HairColor &) { return "HairColor"; },
                                       [](const an::Accessory &) { return "Accessory"; }},
                      trait);
}

[[nodiscard]] auto an::probable_trait_to_str(const an::ProbableTrait &trait) -> std::string {
    return std::visit(
        entt::overloaded{
            [&](const an::ShirtColor &color) {
                return fmt::format("{}: ({}, {}, {})", probable_trait_name_to_str(trait), color.color.r, color.color.g,
                                   color.color.b);
            },
            [&](const an::PantsColor &color) {
                return fmt::format("{}: ({}, {}, {})", probable_trait_name_to_str(trait), color.color.r, color.color.g,
                                   color.color.b);
            },
            [&](const an::HairColor &color) {
                return fmt::format("{}: ({}, {}, {})", probable_trait_name_to_str(trait), color.color.r, color.color.g,
                                   color.color.b);
            },
            [&](const an::Accessory &accessory) { return fmt::format("Accessory: ({})", accessory.accessory_num); },
        },
        trait);
}

[[nodiscard]] auto generate_bool_vec(std::size_t size, std::uint32_t num_ones) -> std::vector<bool> {
    auto vec = std::vector<bool>(size, false);
    for (auto i = 0u; i < num_ones; i++) {
        vec[i] = true;
    }

    return vec;
}

[[nodiscard]] auto an::get_random_shirt_color() -> an::ShirtColor {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution<std::uint8_t> dist{0, 255};

    return {dist(gen), dist(gen), dist(gen)};
}

[[nodiscard]] auto an::get_random_pants_color() -> an::PantsColor {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution<std::uint8_t> dist{0, 255};

    return {dist(gen), dist(gen), dist(gen)};
}

[[nodiscard]] auto an::get_random_hair_color() -> an::HairColor {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution<std::uint8_t> dist{0, 255};

    return {dist(gen), dist(gen), dist(gen)};
}

[[nodiscard]] auto an::get_random_accessory() -> an::Accessory {
    // TODO:
    return {0};
}

[[nodiscard]] auto get_vec_of_random_probable_traits() -> std::vector<an::ProbableTrait> {
    auto probable_traits = std::vector<an::ProbableTrait>{};
    probable_traits.emplace_back(std::in_place_index<0>, an::get_random_shirt_color());
    probable_traits.emplace_back(std::in_place_index<1>, an::get_random_pants_color());
    probable_traits.emplace_back(std::in_place_index<2>, an::get_random_hair_color());
    probable_traits.emplace_back(std::in_place_index<3>, an::get_random_accessory());

    return probable_traits;
}

void apply_probable_trait(an::CharacterTraits &traits, const an::ProbableTrait &trait) {
    std::visit(entt::overloaded{
                   [&](const an::ShirtColor &color) { traits.shirt_color = color; },
                   [&](const an::PantsColor &color) { traits.pants_color = color; },
                   [&](const an::HairColor &color) { traits.hair_color = color; },
                   [&](const an::Accessory &accessory) {
                       // TODO: This does not work
                       traits.accesories_mask |= (1u << accessory.accessory_num);
                   },
               },
               trait);
}

auto an::CharacterGenerator::get_original_character_traits() const -> std::span<const CharacterTraits> {
    return generated_characters;
}

auto an::CharacterGenerator::new_day(const DayConfig &config) -> ResolvedDay {
    static std::mt19937 gen{seed};
    AnomalyTraits anomaly_traits{};

    auto probable_traits_mask = generate_bool_vec(config.num_probable_traits, config.num_used_probable_traits);
    std::shuffle(probable_traits_mask.begin(), probable_traits_mask.end(), gen);
    const auto random_probable_traits = get_vec_of_random_probable_traits();
    for (auto i = 0u; i < probable_traits_mask.size(); i++) {
        if (probable_traits_mask[i]) {
            anomaly_traits.probable_traits.push_back(random_probable_traits.at(i));
            fmt::println("Trait[{}]: {}", i, probable_trait_to_str(random_probable_traits.at(i)));
        }
    }

    const auto num_anomalies = config.num_anomalies;
    auto dist = std::bernoulli_distribution{emplace_probable_trait_chance};
    auto new_characters = generated_characters;
    for (auto &character : new_characters | std::ranges::views::take(num_anomalies)) {
        for (const auto &random_probable_trait : random_probable_traits) {
            apply_probable_trait(character, random_probable_trait);
        }
    }
    for (auto &character : new_characters | std::ranges::views::drop(num_anomalies)) {
        for (const auto &random_probable_trait : random_probable_traits) {
            if (dist(gen)) {
                apply_probable_trait(character, random_probable_trait);
            }
        }
    }

    return {std::move(anomaly_traits), std::move(new_characters)};
}

[[nodiscard]] auto an::get_config_for_day(std::uint32_t day_number) -> an::DayConfig { return {3, 5, 3, 1}; }
