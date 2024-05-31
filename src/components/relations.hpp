#pragma once

#include "components/velocity.hpp"
#include <entt.hpp>

namespace an {

struct Parented {
    entt::entity parent;
};

void destroy_unparented(entt::registry &registry);

/// Zakłada, że wszystkie parenty są validne
/// Najlepiej wcześniej puścić "destroy_unparented", żeby się nie zdziwić
void propagate_parent_transform(entt::registry &registry);

} // namespace an