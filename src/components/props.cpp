#include "props.hpp"
#include "assets/asset_manager.hpp"
#include "components/sprite.hpp"
#include "gui/inspector.hpp"
#include <sstream>

const char *an::get_prop_name(const an::PropType type) {
    switch (type) {
    case an::PropType::TREE1:
        return "Tree1";
    case an::PropType::TREE2:
        return "Tree2";
    case an::PropType::ROCK1:
        return "Rock1";
    case an::PropType::ROCK2:
        return "Rock2";
    case an::PropType::BUSH:
        return "Bush";
    case an::PropType::BIN:
        return "Bin";
    case an::PropType::LAMP:
        return "Lamp";
    case an::PropType::BENCH:
        return "Bench";
    default:
        return "None";
    }
}

an::PropType an::get_prop_type(const std::string &name) {
    if (name == "Tree1") {
        return an::PropType::TREE1;
    } else if (name == "Tree2") {
        return an::PropType::TREE2;
    } else if (name == "Rock1") {
        return an::PropType::ROCK1;
    } else if (name == "Rock2") {
        return an::PropType::ROCK2;
    } else if (name == "Bush") {
        return an::PropType::BUSH;
    } else if (name == "Bin") { 
        return an::PropType::BIN;
    } else if (name == "Lamp") {
        return an::PropType::LAMP;
    } else if (name == "Bench") {
        return an::PropType::BENCH;
    } else {
        return an::PropType::CNT;
    }
}

an::TextureEnum an::get_prop_texture(const an::PropType type) {
    return (TextureEnum)((int)type + (int)TextureEnum::TREE1);
}

an::StaticBody an::get_prop_collision(const an::PropType type) {
    switch (type) {
    case an::PropType::TREE1:
        return an::StaticBody{{-15.f, -15.f}, {30.f, 30.f}};
    case an::PropType::TREE2:
        return an::StaticBody{{-15.f, -15.f}, {30.f, 30.f}};
    case an::PropType::ROCK1:
        return an::StaticBody{{-15.f, -15.f}, {30.f, 30.f}};
    case an::PropType::ROCK2:
        return an::StaticBody{{-0.f, -0.f}, {0.f, 0.f}};
    case an::PropType::BUSH:
        return an::StaticBody{{-0.f, -0.f}, {0.f, 0.f}};
    case an::PropType::BIN:
        return an::StaticBody{{-15.f, -15.f}, {30.f, 30.f}};
    case an::PropType::LAMP:
        return an::StaticBody{{-15.f, -15.f}, {30.f, 30.f}};
    case an::PropType::BENCH:
        return an::StaticBody{{-15.f, -15.f}, {30.f, 30.f}};
    default:
        return an::StaticBody{{-15.f, -15.f}, {30.f, 30.f}};
    }
}

void an::update_props(entt::registry &registry) {
    auto view = registry.view<an::Prop, an::Drawable>();
    for (auto &&[entity, prop, draw] : view.each()) {
        if (prop.update) {
            registry.erase<an::Drawable>(entity);
            registry.erase<an::DebugName>(entity);
            emplace<an::Sprite>(registry, entity, an::get_prop_texture(prop.type));
            an::emplace<an::DebugName>(registry, entity, an::get_prop_name(prop.type));
            prop.update = false;
        }
    }
}

auto an::spawn_prop(entt::registry &registry) -> entt::entity {
    static unsigned num = 0;
    auto entity = registry.create();
    emplace<an::Prop, an::PropType>(registry, entity, static_cast<an::PropType>(0));
    auto &prop = registry.get<an::Prop>(entity);
    registry.remove<an::DebugName>(entity);
    an::emplace<an::DebugName>(registry, entity, an::get_prop_name(prop.type) + std::to_string(num++));
    auto &transform = registry.get<an::LocalTransform>(entity);
    transform.transform.position = GetScreenToWorld2D(GetMousePosition(), registry.ctx().get<Camera2D>());
    return entity;
}

void an::save_props(entt::registry &registry) {
    auto view = registry.view<an::Prop, an::GlobalTransform>();
    std::ofstream MyFile("props.dat");
    for (auto &&[entity, prop, transform] : view.each()) {
        MyFile << static_cast<int>(prop.type) << " " << transform.transform.position.x << " "
               << transform.transform.position.y << std::endl;
    }
}

std::ifstream an::get_ifstream(const char *filename) { return std::ifstream(filename); }

void an::load_props(entt::registry &registry, std::ifstream strm) {
    std::string line;
    while (getline(strm, line)) {
        auto entity = registry.create();
        std::stringstream ss(line);
        std::string tmp;
        getline(ss, tmp, ' ');
        emplace<an::Prop, an::PropType>(registry, entity, static_cast<an::PropType>(std::stoi(tmp)));
        auto &transform = registry.get<an::LocalTransform>(entity);
        getline(ss, tmp, ' ');
        transform.transform.position.x = std::stof(tmp);
        getline(ss, tmp, ' ');
        transform.transform.position.y = std::stof(tmp);
    }
}

void an::Prop::inspect(entt::registry &registry, entt::entity entity) {
    const size_t minimum = 0;
    const size_t maximum = static_cast<size_t>(PropType::CNT) - 1;
    ImGui::Text("%s", get_prop_name(type));
    ImGui::SliderScalar("Type", ImGuiDataType_U8, &type, &minimum, &maximum);
    ImGui::Checkbox("Update", &update);
}

template <>
void an::emplace<an::Prop, an::PropType>(entt::registry &registry, entt::entity entity, const PropType &type) {
    auto prop = registry.emplace<Prop>(entity, type);
    emplace<Sprite>(registry, entity, get_prop_texture(type));
    emplace<GlobalTransform>(registry, entity);
    emplace<Visible>(registry, entity);
    emplace<DebugName>(registry, entity, get_prop_name(type));
    emplace<StaticBody>(registry, entity, get_prop_collision(type));
}

template <> void an::emplace<an::Prop, an::Prop>(entt::registry &registry, entt::entity entity, const Prop &prop) {
    registry.emplace<Prop>(entity, prop.type);
    emplace<Sprite>(registry, entity, get_prop_texture(prop.type));
    emplace<GlobalTransform>(registry, entity);
    emplace<Visible>(registry, entity);
    safe_emplace<DebugName>(registry, entity, get_prop_name(prop.type));
    emplace<StaticBody>(registry, entity, get_prop_collision(prop.type));
}

[[nodiscard]] auto an::get_random_prop() -> Prop {
    static std::random_device rd{};
    static std::mt19937 gen{rd()};
    static std::uniform_int_distribution<std::uint8_t> dist{0, static_cast<std::uint8_t>(PropType::CNT) - 1};

    return Prop{.type = static_cast<PropType>(dist(gen)), .update = false};
}
