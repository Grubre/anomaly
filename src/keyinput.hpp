#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <entt.hpp>

namespace an {
enum class KeyboardEvent : uint8_t { PRESS, RELEASE, UP, DOWN };
class KeyManager {
  public:
    using callback_t = std::function<void()>;
    using raylib_key_t = int;
    using subscriber_id_t = std::uint32_t;

  private:
    struct Subscriber {
        KeyboardEvent type;
        callback_t callback;
        subscriber_id_t id;
    };
    [[nodiscard]] static auto make_subscriber(callback_t &&callback, KeyboardEvent type) -> Subscriber;

  public:
    auto subscribe(KeyboardEvent type, raylib_key_t key, callback_t &&callback) -> subscriber_id_t;
    void unsubscribe(subscriber_id_t id);

    std::unordered_map<raylib_key_t, std::vector<Subscriber>> subscribers;
};

void notify_keyboard_press_system(KeyManager &manager);

} // namespace an
