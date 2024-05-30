#pragma once

#include "components/common.hpp"
#include <fmt/printf.h>
#include <imgui.h>
#include <entt.hpp>
#include <imgui_internal.h>
#include <optional>
#include <variant>

namespace an {
struct DebugName {
    std::string name;
};

template <typename T>
concept InspectableComponent = requires(T t, entt::registry &registry, entt::entity entity) {
    { T::name } -> std::convertible_to<const char *>;
    { t.inspect(registry, entity) };
} || requires() {
    { T::name } -> std::convertible_to<const char *>;
    { T::inspect() };
    { std::is_empty_v<T> };
};

template <InspectableComponent T> void inspect(T &t, entt::registry &registry, entt::entity entity) {
    if constexpr (std::is_empty_v<T>) {
        T::inspect();
    } else {
        t.inspect(registry, entity);
    }
}

template <InspectableComponent... Component> struct Inspector {
    static constexpr auto EntityDragDropTypeName = "Entity";
    static constexpr auto ComponentDragdropTypeName = "Component";
    explicit Inspector(entt::registry *registry) : registry(registry) {}

    std::optional<entt::entity> current_entity;
    void draw_gui() {
        ImGui::Begin("Inspector");

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // Optional: reduce spacing between items

        // Split main window horizontally
        ImGui::BeginGroup();
        display_entity_list(ImVec2(ImGui::GetContentRegionAvail().x * (2.f / 5.f), ImGui::GetContentRegionAvail().y));
        ImGui::EndGroup();

        ImGui::SameLine();

        if (!current_entity) {
            ImGui::Text("No entity selected");
            ImGui::PopStyleVar();
            ImGui::End();
            return;
        }

        // Begin right column
        ImGui::BeginGroup();
        ImVec2 rightColumnSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

        const auto entity_list_size = ImVec2(rightColumnSize.x, rightColumnSize.y * 3.f / 5.f);
        display_entity_info(entity_list_size);

        // Bottom part of the right column
        display_component_creator();

        ImGui::PopStyleVar();
        ImGui::End();
    }

  private:
    void display_entity_list(ImVec2 size) {
        ImGui::BeginChild("Entity list", size, 1);
        ImGui::SeparatorText("Toggle components");

        auto i = 0u;
        ([&]() { ImGui::Checkbox(Component::name, &component_filter[i++]); }(), ...);

        ImGui::SeparatorText("Entity List");
        for (auto entity : registry->view<entt::entity>()) {
            i = 0u;
            bool has_toggled_components =
                ([&]() { return !component_filter[i++] || registry->all_of<Component>(entity); }() && ...);

            if (!has_toggled_components) {
                continue;
            }

            display_entity_list_entry(entity);
        }

        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("Create new entity")) {
                const auto entity = registry->create();
                an::emplace<an::DebugName>(*registry, entity, "New entity");
            }
            ImGui::EndPopup();
        }

        ImGui::EndChild();
    }

    void display_component_creator() {

        ImGui::PushID((int)generated_component.index());
        ImGui::BeginChild("Component creator", ImVec2(0, 0), 1);
        ImGui::SeparatorText("Component creator");

        static const char *chosen_component_name = "None";
        ImVec2 text_size = ImGui::CalcTextSize(chosen_component_name);

        ImGui::Text("Chosen type: ");
        ImGui::SameLine();
        if (ImGui::Button(chosen_component_name, ImVec2(text_size.x + 10, 0))) {
            ImGui::OpenPopup(chosen_component_name);
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload(ComponentDragdropTypeName, &generated_component, sizeof(generated_component));
            ImGui::Text(chosen_component_name);
            ImGui::EndDragDropSource();
        }

        ImVec2 button_pos = ImGui::GetItemRectMin();
        ImVec2 button_size = ImGui::GetItemRectSize();
        ImGui::SetNextWindowPos(ImVec2(button_pos.x, button_pos.y + button_size.y), ImGuiCond_Appearing);

        if (ImGui::BeginPopup(chosen_component_name)) {
            uint32_t i = 0;
            iterate_components([&]<InspectableComponent Comp>() {
                if (ImGui::MenuItem(Comp::name, NULL, generated_component.index() == i)) {
                    generated_component = Comp{};
                    chosen_component_name = Comp::name;

                    ImGui::CloseCurrentPopup();
                }
                i++;
            });
            ImGui::EndPopup();
        }

        ImGui::SeparatorText("Component data");

        std::visit(entt::overloaded{
                       [&](std::monostate) { ImGui::Text("No component selected"); },
                       [&](auto &component) { inspect(component, *registry, entt::null); },
                   },
                   generated_component);

        ImGui::EndChild();
        ImGui::EndGroup();
        ImGui::PopID();
    }

    void display_entity_list_entry(entt::entity entity) {
        auto *const name = registry->try_get<DebugName>(entity);

        const auto text = fmt::format("{} ({})", (int)entity, name != nullptr ? name->name : "unknown");

        if (ImGui::Selectable(text.c_str(), current_entity == entity, ImGuiSelectableFlags_SelectOnClick)) {
            if (ImGui::IsMouseClicked(0)) {
                current_entity = entity;
            }
        }

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::SeparatorText(text.c_str());
            iterate_components([&]<InspectableComponent Comp>() {
                if (!registry->all_of<Comp>(entity)) {
                    return;
                }
                ImGui::Text(Comp::name);
            });
            ImGui::EndTooltip();
        }
    }

    void iterate_components(auto &&f) { (f.template operator()<Component>(), ...); }

    void display_entity_info(ImVec2 size) {
        entt::entity entity = *current_entity;

        ImGui::BeginChild("Entity Inspector", size, 1);
        if (ImGui::Button("Delete")) {
            registry->destroy(entity);
            current_entity = std::nullopt;
            ImGui::EndChild();
            return;
        }

        ImGui::SeparatorText("Entity data");

        ImGui::BeginGroup();
        ImGui::Text("Entity: %d", (int)entity);
        if (auto *name = registry->try_get<DebugName>(entity)) {
            ImGui::Text("Name: %s", name->name.c_str());
        }
        ImGui::EndGroup();

        display_components(entity);

        ImGui::EndChild();

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(ComponentDragdropTypeName)) {
                uint32_t i = 0u;
                (
                    [&]() {
                        fmt::println("name: {}", Component::name);
                        if (i++ != generated_component.index() ||
                            std::holds_alternative<std::monostate>(generated_component)) {
                            return;
                        }

                        an::safe_emplace<Component>(*registry, entity, std::get<Component>(generated_component));
                    }(),
                    ...);
            }
            ImGui::EndDragDropTarget();
        }
    }

    void display_components(entt::entity entity) {
        ImGui::SeparatorText("Components");

        iterate_components([&]<InspectableComponent Comp>() {
            if (!registry->all_of<Comp>(entity)) {
                return;
            }

            ImGui::PushID(Comp::name);

            bool header_open = ImGui::CollapsingHeader(Comp::name);

            // Create a popup menu on right-click
            if (ImGui::BeginPopupContextItem("component_context_menu")) {
                if (ImGui::MenuItem("Delete")) {
                    registry->remove<Comp>(entity);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (header_open) {
                if constexpr (std::is_empty_v<Comp>) {
                    Comp::inspect();
                } else {
                    auto &component = registry->get<Comp>(entity);
                    component.inspect(*registry, entity);
                }
            }

            ImGui::PopID();
        });
    }

    std::variant<Component..., std::monostate> generated_component = std::monostate{};
    entt::registry *registry;
    std::array<bool, sizeof...(Component)> component_filter{};
};

} // namespace an
