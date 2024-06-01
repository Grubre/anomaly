#pragma once 

#include "assets/asset_manager.hpp"
#include "fmt/base.h"
#include <unordered_set>
#include <entt.hpp>
#include <imgui.h>

namespace an {

enum class EqItem {
    STICK = 0,
    SEESAW,
    HAMMER,
    NAIL,
};

struct Equipment {
    std::unordered_set<EqItem> eq;

    void insert(EqItem item) {
        eq.insert(item);
    }

    bool has(EqItem item) const {
        return eq.contains(item);
    }

    static constexpr auto name = "Equipment";

    void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
        if (ImGui::Button("Insert All")) {
            insert(EqItem::STICK);
            insert(EqItem::SEESAW);
            insert(EqItem::HAMMER);
            insert(EqItem::NAIL);
        }

        for (const auto &s : eq) {
            ImGui::Text("%d ", (int)s);
            ImGui::SameLine();
        }
    }
};

inline void show_equipment(entt::registry& registry, entt::entity player) {
    auto &equipment = registry.get<Equipment>(player);
    auto &asset_manager = registry.ctx().get<AssetManager>();

    ImGui::Begin("Equipment", nullptr);
    for(const auto &item : equipment.eq) {
        auto id = (TextureEnum)((int)TextureEnum::STICK + (int)item);
        auto *item_texture = asset_manager.get_texture_ptr(id);
        ImGui::Image((void *)item_texture, {100.f, 64.f});
        ImGui::SameLine(0.f, 20.f);
    }

    ImGui::End();
}

}