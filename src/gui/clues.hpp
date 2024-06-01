#pragma once

#include "character_generator.hpp"

namespace an {

inline void clues_gui(ResolvedDay &day) {
    ImGui::SeparatorText(fmt::format("Probable traits ({} of {})", day.num_used_probable_traits,
                                     std::variant_size<an::ProbableTrait>().value)
                             .c_str());
    for (auto &trait : day.anomaly_traits.probable_traits) {
        ImGui::Text("%s: ", probable_trait_name_to_str(trait).data());
        ImGui::SameLine();
        std::visit(entt::overloaded{
                       [&](const auto &color) {
                           ImGui::ColorButton("Color",
                                              ImVec4((float)color.color.r / 255.f, (float)color.color.g / 255.f,
                                                     (float)color.color.b / 255.f, 1.f));
                       },
                       [&](const an::ParticleTrait &particle) { ImGui::Text("Particle: %d", (int)particle.type); },
                   },
                   trait);
    }
    ImGui::SeparatorText("Guaranteed traits");
    for (auto &trait : day.anomaly_traits.guaranteed_traits) {
        ImGui::Text("%s", guaranteed_trait_to_str(trait).c_str());
    }
}

} // namespace an
