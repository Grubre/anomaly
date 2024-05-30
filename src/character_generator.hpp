#pragma once

#include <raylib.h>
#include <bitset>
#include <array>
#include <cstdint>
#include <span>
#include <type_traits>
#include <variant>
#include <vector>
#include "components/props.hpp"

namespace an {
using accesories_mask_t = std::bitset<8>;

constexpr std::array possible_shirt_colors{
    RED,
    GREEN,
    BLUE,
};

constexpr std::array possible_hair_colors{
    BLACK,
};

using ShirtColor = Color;
using PantsColor = Color;
using HairColor = Color;

struct CharacterTraits {
    ShirtColor shirt_color{};
    PantsColor pants_color{};
    HairColor hair_color{};

    accesories_mask_t accesories_mask{};
};

struct DayConfig {
    std::uint32_t num_guaranteed_traits;
    std::uint32_t num_probable_traits;
    std::uint32_t num_used_probable_traits;
    std::uint32_t num_anomalies;
};

using ProbableTrait = std::variant<ShirtColor, PantsColor, HairColor>;

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

using GuaranteedTrait = std::variant<Avoid, TwitchNear>;

struct AnomalyTraits {
    std::vector<ProbableTrait> probable_traits;
    std::vector<GuaranteedTrait> guaranteed_traits;
};

[[nodiscard]] auto get_config_for_day(std::uint32_t day_number) -> DayConfig;

class CharacterGenerator {
  public:
    constexpr static float accessory_gen_chance = 0.1f;

    explicit CharacterGenerator(std::uint32_t seed, std::uint32_t characters_cnt) : seed(seed) {
        generate_characters(characters_cnt);
    }

    [[nodiscard]] auto new_day(const DayConfig &config) -> AnomalyTraits;
    [[nodiscard]] auto get_original_character_traits() const -> std::span<const CharacterTraits>;

  private:
    void generate_characters(std::uint32_t characters_cnt);

    CharacterTraits anomaly{};
    std::uint32_t seed{};

    std::vector<CharacterTraits> generated_characters{};
};

} // namespace an
