#include <algorithm>
#include <raylib.h>
#include "keyinput.hpp"

[[nodiscard]] auto bh::KeyManager::make_subscriber(callback_t &&callback, KeyboardEvent type) -> Subscriber {
    static subscriber_id_t id = 0;
    return Subscriber{type, std::move(callback), id++};
}

auto bh::KeyManager::subscribe(KeyboardEvent type, raylib_key_t key, callback_t &&callback) -> subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    subscribers[key].push_back(std::move(subscriber));
    return id;
}

void bh::KeyManager::unsubscribe(subscriber_id_t id) {
    for (auto &[key, subs] : subscribers) {
        subs.erase(std::remove_if(subs.begin(), subs.end(), [id](const auto &sub) { return sub.id == id; }),
                   subs.end());
    }
}

void bh::notify_keyboard_press_system(bh::KeyManager &manager) {
    for (const auto &key_val : manager.subscribers) {
        for (const auto &sub : key_val.second) {
            using ST = KeyboardEvent;
            if ((sub.type == ST::PRESS && IsKeyPressed(key_val.first)) ||
                (sub.type == ST::RELEASE && IsKeyReleased(key_val.first)) ||
                (sub.type == ST::UP && IsKeyUp(key_val.first)) ||
                (sub.type == ST::DOWN && IsKeyDown(key_val.first))) {
                sub.callback();
            }
        }
    }
}
