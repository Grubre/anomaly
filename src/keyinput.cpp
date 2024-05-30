#include <algorithm>
#include <raylib.h>
#include "keyinput.hpp"

[[nodiscard]] auto an::KeyManager::make_subscriber(callback_t &&callback, KeyboardEvent type) -> Subscriber {
    static subscriber_id_t id = 0;
    return Subscriber{type, std::move(callback), id++};
}

auto an::KeyManager::subscribe(KeyboardEvent type, KeyboardKey key, callback_t &&callback) -> subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    subscribers[key].push_back(std::move(subscriber));
    return id;
}

void an::KeyManager::unsubscribe(subscriber_id_t id) {
    for (auto &[key, subs] : subscribers) {
        subs.erase(std::remove_if(subs.begin(), subs.end(), [id](const auto &sub) { return sub.id == id; }),
                   subs.end());
    }
}
auto an::KeyManager::subscribe(an::KeyboardEvent type, an::KeyEnum key,
                               an::KeyManager::callback_t &&callback) -> an::KeyManager::subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    subscribers[keys.at(static_cast<size_t>(key))].push_back(std::move(subscriber));
    return id;
}
auto an::KeyManager::get_key(const an::KeyEnum id) -> KeyboardKey {
    return keys.at(static_cast<size_t>(id));
}
void an::KeyManager::assign_key(const KeyboardKey key, const KeyEnum id) {
    keys.at(static_cast<size_t>(id)) = key;
}

void an::notify_keyboard_press_system(an::KeyManager &manager) {
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
