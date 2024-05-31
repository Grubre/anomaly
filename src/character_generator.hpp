#pragma once

#include <raylib.h>
#include <bitset>
#include <array>
#include <cstdint>
#include <span>
#include <variant>
#include <vector>
#include "components/props.hpp"

namespace an {
using accesories_mask_t = std::bitset<8>;

constexpr std::array possible_shirt_colors{
    RED, GREEN, BLUE, YELLOW, VIOLET, ORANGE,
};

constexpr std::array possible_hair_colors{BLACK, YELLOW, RED, BROWN};

using ShirtColor = Color;
using PantsColor = Color;
using HairColor = Color;
struct Accessory {
    std::uint32_t accessory_num;
};

[[nodiscard]] auto get_random_shirt_color() -> ShirtColor;
[[nodiscard]] auto get_random_pants_color() -> PantsColor;
[[nodiscard]] auto get_random_hair_color() -> HairColor;
[[nodiscard]] auto get_random_accessory() -> Accessory;

struct CharacterTraits {
    ShirtColor shirt_color{};
    PantsColor pants_color{};
    HairColor hair_color{};

    accesories_mask_t accesories_mask{};
};

template <typename T>
concept ReactionToProp = requires(T t) {
    { t.type } -> std::same_as<PropType>;
};

struct Avoid {
    PropType type;
};

struct TwitchNear {
    PropType type;
};

using ProbableTrait = std::variant<ShirtColor, PantsColor, HairColor, Accessory>;
using GuaranteedTrait = std::variant<Avoid, TwitchNear>;

struct AnomalyTraits {
    std::vector<ProbableTrait> probable_traits;
    std::vector<GuaranteedTrait> guaranteed_traits;
};

struct DayConfig {
    std::uint32_t num_guaranteed_traits{};
    std::uint32_t num_probable_traits{};
    std::uint32_t num_used_probable_traits{};
    std::uint32_t num_anomalies{};
};

struct ResolvedDay {
    AnomalyTraits anomaly_traits;
    std::vector<CharacterTraits> characters;
};

[[nodiscard]] auto get_config_for_day(std::uint32_t day_number) -> DayConfig;

class CharacterGenerator {
  public:
    constexpr static float accessory_gen_chance = 0.1f;
    constexpr static double emplace_probable_trait_chance = 0.5;

    explicit CharacterGenerator(std::uint32_t seed, std::uint32_t characters_cnt) : seed(seed) {
        generate_characters(characters_cnt);
    }

    // first `config.num_anomalies` of returned `ResolvedDay::characters` are anomalies
    [[nodiscard]] auto new_day(const DayConfig &config) -> ResolvedDay;
    [[nodiscard]] auto get_original_character_traits() const -> std::span<const CharacterTraits>;

  private:
    void generate_characters(std::uint32_t characters_cnt);

    CharacterTraits anomaly{};
    std::uint32_t seed{};

    std::vector<CharacterTraits> generated_characters{};
};

} // namespace an
