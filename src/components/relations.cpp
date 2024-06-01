#include "relations.hpp"
namespace an {

void destroy_unparented(entt::registry &registry) {
    auto view = registry.view<Parented>();

    for (auto &&[self, parent] : view.each()) {
        if (!registry.valid(parent.parent)) {
            registry.destroy(self);
        }
    }
}
void propagate_parent_transform(entt::registry &registry) {
    auto view = registry.view<an::GlobalTransform, an::LocalTransform>();

    for (auto &&[self, global, local] : view.each()) {
        if (registry.all_of<Parented>(self)) {
            auto parent = registry.get<an::Parented>(self);
            auto parent_transform = registry.get<an::GlobalTransform>(parent.parent);
            global.transform = parent_transform.transform.combine(local.transform);
        } else {
            global.transform = local.transform;
        }
    }
}
} // namespace an
