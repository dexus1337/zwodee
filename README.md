# Zwodee Engine

Zwodee is a high-performance, modular 2D game engine written in **C++23**, using **SDL3** for modern cross-platform windowing, hardware-accelerated rendering, and audio hardware access. Designed around a multi-threaded architecture with a decoupled **128Hz fixed-timestep simulation thread** and a lock-free double-buffered render pipeline, Zwodee delivers smooth graphics, deterministic physics, and robust offline tooling for 2D game development.

---

## Key Features

- **Decoupled 128Hz Fixed-Timestep Simulation**: Logic, physics, and AI run on a dedicated simulation thread locked at 128 ticks per second for deterministic game state updates regardless of monitor refresh rates.
- **Lock-Free Double-Buffered Frame Snapshots**: Multi-threaded synchronization via atomic front/back buffer snapshot swapping (`render_snapshot`) ensuring zero render lag or tearing between threads.
- **Render Frame Interpolation**: The render loop calculates fractional interpolation alpha values between simulation ticks to render butter-smooth motion at any frame rate.
- **High-Precision Hybrid FPS Limiter**: Configurable frame rate caps supporting VSync, Unlocked mode, and specific targets (60, 144, 240, 360, and 480 FPS) using high-precision hardware counters combined with hybrid sleep and busy-wait precision.
- **Hardware-Accelerated 2D Rendering Engine**:
  - Direct SDL3 renderer integration supporting custom color clearing, texture scale modulation, and sprite flipping (horizontal and vertical).
  - DirectDraw Surface (**DDS**) texture loader alongside standard **BMP** texture loading for fast, compressed GPU texture streaming.
  - Custom hardware-accelerated screen blur filters and transparent color overlays.
- **Oversampled TrueType Font Engine**: Integrated TTF font rendering using oversampled glyph rasterization for crisp, anti-aliased text scaling with custom color and scale parameters.
- **In-Engine Developer Console**: Built-in interactive console overlay with input buffer capturing and command popping interface (`pop_console_command`) for real-time debugging and telemetry.
- **Dedicated Audio Control System**: OOP `audio_manager` supporting multi-channel WAV sound effect preloading, caching, playback, and runtime global mute controls (`set_muted`).
- **Unified AABB Collision Engine**: Axis-Aligned Bounding Box collision detection built into the core `game_object` class. Grid tiles, static objects, and dynamic entities share unified collision checks (`collides_with`) and response hooks (`on_collision`).
- **Rich Object-Oriented Entity Hierarchy**:
  - `game_object`: Base transform, velocity, and bounding box component.
  - `textured_object` / `static_object`: Renderable static geometry and tile elements.
  - `animated_object` & `animator`: State-machine driven sprite animation manager supporting configurable frame durations, multi-state transitions, and looping.
  - `entity_player`: Controller-driven player entity with keyboard input processing.
  - `entity_ai`: Autonomous AI entity with boundary awareness and stateful directional roaming.
- **Modular Level & Tilemap System**:
  - Proprietary Binary Level Format (**`.zwl`**): Packed binary level headers (`level_header`), static tile flags (`binary_tile`), and dynamic entity spawn definitions (`binary_entity`).
  - `tile_level`: Grid-based tilemap renderer and entity lifecycle manager.
  - `level_loader`: Binary file parser constructing executable `tile_level` instances at runtime.
  - `level_manager`: Level lifecycle controller handling level transitions and scene state.
- **Offline Engine Tooling Suite**:
  - `zwodee-level-compiler`: Command-line tool converting human-readable JSON level definitions into `.zwl` binary maps.
  - `zwodee-texture-compiler`: Command-line tool converting PNG, JPG, and BMP assets into compressed GPU-ready `.dds` textures.
- **Automated Test Suite**: Full unit test coverage powered by **GoogleTest** (`zwodee-test`) verifying physics, entity logic, tilemaps, level deserialization, and animators.

---

## Repository Structure

```
zwodee/
├── cmake/                  # CMake modules & GoogleTest setup
├── include/                # Public Engine Headers (SDK interface)
│   ├── api/                # API export / import macros
│   ├── audio/              # Sound effect definitions & audio manager
│   ├── core/               # Engine lifecycle, 128Hz loop, dev console & input
│   ├── entities/           # game_object hierarchy, animators, player & AI
│   ├── graphics/           # Renderer, window, DDS format, font & render snapshots
│   └── levels/             # Base level, tile_level, .zwl format & level loader
├── source/                 # Engine Implementation Source Files
├── test/                   # GoogleTest unit test suite
├── tools/                  # Offline Engine Tools & Utilities
│   ├── level-compiler/     # JSON to .zwl binary level compiler
│   └── texture-compiler/   # Image to DDS texture compiler
├── CMakeLists.txt          # Root CMake build configuration
└── CMakePresets.json       # Cross-platform CMake build presets
```

---

## Build Prerequisites

To compile Zwodee and its toolset, ensure your development environment includes:
1. **CMake** (v3.20 or higher)
2. **Ninja** build generator (recommended) or MSVC / GCC / Clang
3. A **C++23 compatible compiler** (MSVC 2022+, GCC 13+, Clang 16+)
4. **SDL3** development library (installed and detectable by CMake)

---

## Building the Project

Zwodee provides CMake Presets for Windows (MSVC & MinGW) and Linux build configurations.

### 1. Build Engine & Tools (Windows x64 Debug)
```bash
cmake --preset windows-x64-debug
cmake --build out/build/windows-x64-debug
```

### 2. Build Engine & Tools (Windows x64 Release)
```bash
cmake --preset windows-x64-release
cmake --build out/build/windows-x64-release
```

### 3. Build Engine & Tools (Linux x64 Release)
```bash
cmake --preset linux-x64-release
cmake --build out/build/linux-x64-release
```

The output binaries (`zwodee.dll` / `libzwodee.so`, `zwodee-test`, `zwodee-level-compiler`, and `zwodee-texture-compiler`) will be generated inside the build target `bin/` and `test/` directories.

---

## Running Automated Tests

To execute the unit test suite built with GoogleTest:

```bash
# Run via ctest
ctest --preset all

# Or run the executable directly
./out/build/windows-x64-debug/test/zwodee-test
```

---

## Offline Engine Tools

### 1. Level Compiler (`zwodee-level-compiler`)
Converts JSON level map descriptors into compact, binary `.zwl` level files for high-speed runtime loading.

```bash
zwodee-level-compiler level_design.json level_01.zwl
```

### 2. Texture Compiler (`zwodee-texture-compiler`)
Converts standard image files (PNG, JPG, BMP) into GPU-ready `.dds` textures.

```bash
zwodee-texture-compiler sprite_sheet.png sprite_sheet.dds
```

---

## Quick Start API Guide

*Note: All C++ code examples follow Allman bracket formatting.*

### 1. Initializing & Running the Engine
```cpp
#include "zwodee.hpp"

int main()
{
    // Instantiate engine (Title, Width, Height, VSync)
    zwodee::engine engine("My Zwodee Game", 1280, 720, true);

    // Set high-precision FPS cap (e.g. 144 FPS)
    engine.set_fps_limit(zwodee::engine::fps_limit::fps_144);

    // Start 128Hz simulation thread & decoupled render loop
    engine.run();

    return 0;
}
```

### 2. Custom Level Setup
```cpp
#include "zwodee.hpp"

class MyGameLevel : public zwodee::tile_level
{
public:
    MyGameLevel()
        : tile_level(30, 20)
    {
    }

    void on_enter() override
    {
        // Load background DDS texture
        auto bg = get_renderer().load_dds_texture("assets/background.dds");
        set_background_texture(bg.get());

        // Load player entity texture
        auto player_tex = get_renderer().load_texture("assets/player.bmp");

        // Spawn player entity
        auto player = std::make_unique<zwodee::entity_player>(100.0f, 100.0f, player_tex.get());
        add_entity(std::move(player));
    }

    void tick() override
    {
        // Update tile collisions and dynamic entities at 128Hz
        tile_level::tick();
    }
};
```

### 3. Unified Collision Checking
```cpp
#include "zwodee.hpp"

void check_player_collisions(zwodee::entity_player& player, zwodee::entity_ai& enemy)
{
    // Axis-Aligned Bounding Box (AABB) collision check
    if (player.collides_with(enemy))
    {
        player.take_damage(10);
        player.on_collision(enemy);
    }
}
```

### 4. Interactive Developer Console
```cpp
#include "zwodee.hpp"

void process_console_input(zwodee::engine& engine)
{
    if (engine.is_console_active())
    {
        std::string command;
        while (engine.pop_console_command(command))
        {
            if (command == "godmode")
            {
                // Process in-game console command
            }
        }
    }
}
```

### 5. Audio Playback
```cpp
#include "zwodee.hpp"

void trigger_sound_effect(zwodee::engine& engine)
{
    zwodee::audio_manager& audio = engine.get_audio_manager();

    // Preload WAV sample
    if (audio.load_sound("jump", "assets/sounds/jump.wav"))
    {
        // Play sound effect channel
        audio.play_sound("jump");
    }
}
```

---

## License

Zwodee Engine is open-source software released under the MIT License.
