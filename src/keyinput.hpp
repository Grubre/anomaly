#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <entt.hpp>
#include "assets/asset_manager.hpp"
namespace an {
enum class KeyboardEvent : uint8_t { PRESS, RELEASE, UP, DOWN };
enum class KeyEnum : uint8_t {MOVE_UP,MOVE_DOWN,MOVE_LEFT,MOVE_RIGHT,INTERACT,CNT};
class KeyManager {
  public:
    using callback_t = std::function<void()>;
    using subscriber_id_t = std::uint32_t;
    void assign_key(const KeyboardKey key,const KeyEnum id);
    [[nodiscard]] auto get_key(const KeyEnum id) -> KeyboardKey;
  private:
    std::array<KeyboardKey , static_cast<size_t>(KeyEnum::CNT)> keys;
    struct Subscriber {
        KeyboardEvent type;
        callback_t callback;
        subscriber_id_t id;
    };
    [[nodiscard]] static auto make_subscriber(callback_t &&callback, KeyboardEvent type) -> Subscriber;

  public:
    auto subscribe(KeyboardEvent type, KeyboardKey key, callback_t &&callback) -> subscriber_id_t;
    auto subscribe(KeyboardEvent type, KeyEnum key, callback_t &&callback) -> subscriber_id_t;
    void unsubscribe(subscriber_id_t id);

    std::unordered_map<KeyboardKey , std::vector<Subscriber>> subscribers;
};

void notify_keyboard_press_system(KeyManager &manager);

} // namespace an
