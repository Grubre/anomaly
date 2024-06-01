#include "character_generator.hpp"
#include "components/common.hpp"
#include <random>
#include <ranges>
#include <variant>
#include <vector>

void an::CharacterGenerator::generate_characters(std::uint32_t characters_cnt) {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution dist(0, 2);
    generated_characters.reserve(characters_cnt);

    for (auto i = 0u; i < characters_cnt; i++) {
        CharacterTraits traits{};

        traits.shirt_color = get_random_shirt_color();
        traits.pants_color = get_random_pants_color();
        traits.hair_color = get_random_hair_color();

        if (an::get_uniform_float() < get_particle_chance) {
            traits.particles = get_random_particle();
        }

        traits.shirt_archetype = (an::TextureEnum)((int)an::TextureEnum::CHARACTER_SHIRT_1 + dist(gen));
        traits.pants_archetype = (an::TextureEnum)((int)an::TextureEnum::CHARACTER_PANTS_1 + dist(gen));
        traits.hair_archetype = (an::TextureEnum)((int)an::TextureEnum::CHARACTER_HAIR_1 + dist(gen));

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
                                       [](const an::ParticleTrait &) { return "ParticleTrait"; }},
                      trait);
}

[[nodiscard]] auto an::probable_trait_to_str(const an::ProbableTrait &trait) -> std::string {
    return std::visit(
        entt::overloaded{
            [&](const an::ParticleTrait &particle) { return fmt::format("ParticleTrait: ({})", (int)particle.type); },
            [&](const auto &color) {
                return fmt::format("{}: ({}, {}, {})", probable_trait_name_to_str(trait), color.color.r, color.color.g,
                                   color.color.b);
            },

        },
        trait);
}

[[nodiscard]] auto an::guaranteed_trait_to_str(const GuaranteedTrait &trait) -> std::string {
    return std::visit(
        entt::overloaded{
            [](const an::Avoid &avoid) { return fmt::format("Avoid: ({})", an::get_prop_name(avoid.type)); },
            [](const an::TwitchNear &twitch_near) {
                return fmt::format("TwitchNear: ({})", an::get_prop_name(twitch_near.type));
            },
        },
        trait);
}

[[nodiscard]] auto an::generate_bool_vec(std::size_t size, std::uint32_t num_ones) -> std::vector<bool> {
    auto vec = std::vector<bool>(size, false);
    for (auto i = 0u; i < num_ones; i++) {
        vec[i] = true;
    }

    return vec;
}

[[nodiscard]] auto an::get_random_shirt_color() -> an::ShirtColor {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution<std::uint8_t> dist{0, possible_shirt_colors.size() - 1};

    return ShirtColor{.color = possible_shirt_colors.at(dist(gen))};
}

[[nodiscard]] auto an::get_random_pants_color() -> an::PantsColor {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution<std::uint8_t> dist{0, possible_shirt_colors.size() - 1};

    return PantsColor{.color = possible_shirt_colors.at(dist(gen))};
}

[[nodiscard]] auto an::get_random_hair_color() -> an::HairColor {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution<std::uint8_t> dist{0, possible_hair_colors.size() - 1};

    return HairColor{.color = possible_hair_colors.at(dist(gen))};
}


[[nodiscard]] auto an::get_random_particle() -> an::ParticleTrait {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution<std::uint8_t> dist{0, possible_particles.size() - 1};

    return ParticleTrait{.type = possible_particles.at(dist(gen))};
}

[[nodiscard]] auto an::get_vec_of_random_probable_traits() -> std::vector<an::ProbableTrait> {
    auto probable_traits = std::vector<an::ProbableTrait>{};
    probable_traits.emplace_back(std::in_place_index<0>, an::get_random_shirt_color());
    probable_traits.emplace_back(std::in_place_index<1>, an::get_random_pants_color());
    probable_traits.emplace_back(std::in_place_index<2>, an::get_random_hair_color());
    probable_traits.emplace_back(std::in_place_index<3>, an::get_random_particle());

    return probable_traits;
}

void apply_probable_trait(an::CharacterTraits &traits, const an::ProbableTrait &trait) {
    std::visit(entt::overloaded{
                   [&](const an::ShirtColor &color) { traits.shirt_color = color; },
                   [&](const an::PantsColor &color) { traits.pants_color = color; },
                   [&](const an::HairColor &color) { traits.hair_color = color; },
                   [&](const an::ParticleTrait &particle) { traits.particles = particle; },
               },
               trait);
}

auto an::CharacterGenerator::get_original_character_traits() const -> std::span<const CharacterTraits> {
    return generated_characters;
}

auto an::CharacterGenerator::new_day(const DayConfig &config) -> ResolvedDay {
    static std::mt19937 gen{seed};
    static std::uniform_int_distribution<std::uint32_t> guaranteed_dist{0, num_guaranteed_traits - 1};
    AnomalyTraits anomaly_traits{};

    // TODO: Change it so that this part is done when creating anomalies and
    //       the bitmask is shuffled between creating each, so that they all have
    //       num_used_probable_traits probable traits from the random_probable_traits set
    // get random probable traits
    auto random_probable_traits = get_vec_of_random_probable_traits();

    // FIXME: Could possibly add two identical traits
    // get random guaranteed traits
    for (auto i = 0u; i < config.num_guaranteed_traits; i++) {
        const auto random_prop = an::get_random_prop();

        const auto random_trait_num = guaranteed_dist(gen);

        switch (random_trait_num) {
        case 0:
            anomaly_traits.guaranteed_traits.emplace_back(Avoid{.type = random_prop.type});
            break;
        case 1:
            anomaly_traits.guaranteed_traits.emplace_back(TwitchNear{.type = random_prop.type});
            break;
        default:
            assert(false && "Invalid guaranteed trait number");
        }
    }

    const auto num_anomalies = config.num_anomalies;
    auto dist = std::bernoulli_distribution{emplace_probable_trait_chance};
    auto new_characters = generated_characters;
    auto probable_traits_mask =
        an::generate_bool_vec(std::variant_size<an::ProbableTrait>().value, config.num_used_probable_traits);

    for (auto &character : new_characters | std::ranges::views::take(num_anomalies)) {
        std::shuffle(probable_traits_mask.begin(), probable_traits_mask.end(), gen);
        for (auto i = 0u; i < probable_traits_mask.size(); i++) {
            if (probable_traits_mask[i]) {
                apply_probable_trait(character, random_probable_traits.at(i));
            }
        }
    }

    for (auto &character : new_characters | std::ranges::views::drop(num_anomalies)) {
        for (const auto &random_probable_trait : random_probable_traits) {
            if (dist(gen)) {
                apply_probable_trait(character, random_probable_trait);
            }
        }
    }

    anomaly_traits.probable_traits = std::move(random_probable_traits);

    return {
        .num_used_probable_traits = config.num_used_probable_traits,
        .anomaly_traits = std::move(anomaly_traits),
        .characters = std::move(new_characters),
    };
}

[[nodiscard]] auto an::get_config_for_day(std::uint32_t day_number) -> an::DayConfig { return {3, 3, 1}; }
