#pragma once

#include <raylib.h>
#include <bitset>
#include <array>
#include <cstdint>
#include <span>
#include <variant>
#include <vector>
#include "components/particles.hpp"
#include "components/props.hpp"

namespace an {
using accesories_mask_t = std::bitset<8>;

constexpr std::array possible_shirt_colors{
    RED, GREEN, BLUE, YELLOW, VIOLET, ORANGE,
};

constexpr std::array possible_hair_colors{BLACK, YELLOW, RED, BROWN};
constexpr std::array possible_particles{an::ParticleType::DRUNK, an::ParticleType::STINKY_CHEESE};

struct ShirtColor {
    Color color;
};
struct PantsColor {
    Color color;
};
struct HairColor {
    Color color;
};
struct Accessory {
    std::uint32_t accessory_num;
};
struct ParticleTrait {
    ParticleType type;
};

[[nodiscard]] auto get_random_shirt_color() -> ShirtColor;
[[nodiscard]] auto get_random_pants_color() -> PantsColor;
[[nodiscard]] auto get_random_hair_color() -> HairColor;
[[nodiscard]] auto get_random_accessory() -> Accessory;
[[nodiscard]] auto get_random_particle() -> ParticleTrait;

struct CharacterTraits {
    an::ShirtColor shirt_color{};
    an::PantsColor pants_color{};
    HairColor hair_color{};

    std::optional<ParticleTrait> particles = std::nullopt;

    an::accesories_mask_t accesories_mask{};
};

template <typename T>
concept ReactionToProp = requires(T t) {
    { t.type } -> std::same_as<an::PropType>;
};

struct Avoid {
    PropType type;
};

struct TwitchNear {
    PropType type;
};

using ProbableTrait = std::variant<ShirtColor, PantsColor, HairColor, Accessory, ParticleTrait>;
using GuaranteedTrait = std::variant<Avoid, TwitchNear>;

constexpr auto num_guaranteed_traits = std::variant_size<an::GuaranteedTrait>();
constexpr auto num_probable_traits = std::variant_size<an::ProbableTrait>();

[[nodiscard]] auto probable_trait_name_to_str(const an::ProbableTrait &trait) -> std::string_view;
[[nodiscard]] auto probable_trait_to_str(const ProbableTrait &trait) -> std::string;
[[nodiscard]] auto guaranteed_trait_to_str(const GuaranteedTrait &trait) -> std::string;

struct AnomalyTraits {
    std::vector<ProbableTrait> probable_traits{};
    std::vector<GuaranteedTrait> guaranteed_traits{};
};

struct DayConfig {
    std::uint32_t num_guaranteed_traits{};
    std::uint32_t num_used_probable_traits{};
    std::uint32_t num_anomalies{};
};

struct ResolvedDay {
    std::uint32_t num_used_probable_traits{};
    AnomalyTraits anomaly_traits;
    std::vector<CharacterTraits> characters;
};

[[nodiscard]] auto get_vec_of_random_probable_traits() -> std::vector<an::ProbableTrait>;
[[nodiscard]] auto generate_bool_vec(std::size_t size, std::uint32_t num_ones) -> std::vector<bool>;

[[nodiscard]] auto get_config_for_day(std::uint32_t day_number) -> DayConfig;

class CharacterGenerator {
  public:
    constexpr static float accessory_gen_chance = 0.1f;
    constexpr static double emplace_probable_trait_chance = 0.5;
    constexpr static double get_particle_chance = 0.05;

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
