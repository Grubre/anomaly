#include "assets/assets_loader.hpp"
#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>
#include <fmt/printf.h>
#include <entt.hpp>

void setup_raylib() {
    const auto display = GetCurrentMonitor();
    const int screen_width = GetMonitorWidth(display);
    const int screen_height = GetMonitorHeight(display);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    // SetConfigFlags(FLAG_VSYNC_HINT);

    fmt::println("Resolution is: {}x{}", screen_width, screen_height);
    InitWindow(screen_width, screen_height, "Hello World");
    ToggleFullscreen();
    InitAudioDevice();
}

auto main() -> int {
    fmt::println("hello world!");
    setup_raylib();
    rlImGuiSetup(true);
    auto registry = entt::registry();

    auto base_shader = bh::load_asset(LoadShader, "shaders/base.vs", "shaders/base.fs");

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        rlImGuiBegin();
        ImGui::ShowDemoWindow();
        rlImGuiEnd();

        EndDrawing();
    }
    return 0;
}

