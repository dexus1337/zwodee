# Zwodee Engine

Zwodee is a high-performance, modular 2D game engine built in C++23, using SDL3 for cross-platform rendering and windowing support. It is designed to demonstrate clean object-oriented design, fixed-timestep simulation, multi-threaded interpolation, and state-of-the-art 2D game mechanics.

---

## Key Features

- **Fixed-Timestep Simulation**: Runs a decoupled simulation thread at a fixed tick rate of **128Hz** for deterministic physics and logic.
- **Render Interpolation**: Decoupled rendering loop calculates interpolation alpha values to smooth out frame rate variations.
- **High-Precision Hybrid FPS Limiter**: Accurate frame rate capping supporting VSync, Unlocked, and custom limits (60, 144, 240, 360, 480 FPS) using hardware performance counters with sleep-and-busy-wait hybrid precision.
- **Oversampled Font Engine**: High-fidelity TrueType Font (TTF) rendering using oversampled glyph rasterization to ensure sharp, crisp text at any resolution.
- **Unified AABB Collision Engine**: Bounding box collisions implemented directly in the base `game_object` class, allowing static grid tiles and dynamic entities to share a clean collision interface.
- **Double-Buffered Frame Snapshots**: Lock-free rendering pipeline using atomic swap buffers for frame data synchronization between simulation and render threads.
- **UI Render Overlays & Screen Blur**: Rendering support for semi-transparent UI overlays and hardware-accelerated screen blur filters.
- **Audio Control System**: Dedicated `audio_manager` supporting multi-channel sound effect playback, cache-efficient preloading, and real-time mute controls.
- **Level & State Management**: Modular level interface supporting static tile levels (`tile_level`), dynamic entity instantiation, and runtime background stretching.
- **DDS Texture Support**: Direct-to-GPU compressed DirectDraw Surface (DDS) loading alongside standard BMP textures.

---

## Repository Structure

```
dexus1337/
└── zwodee/                 # Zwodee Engine Core
    ├── include/            # Public Engine Headers
    │   ├── api/            # API Export macros (DLL support)
    │   ├── core/           # Engine loops, input, and lifecycle
    │   ├── entities/       # game_object, entity, animators, player controllers
    │   ├── graphics/       # Renderer, texture wrappers, window
    │   └── levels/         # base level, tile_level, and level manager
    ├── source/             # Core Engine Implementations
    └── CMakeLists.txt      # Engine CMake configuration
```

---

## Build Prerequisites

To compile Zwodee, ensure your system has:
1. **CMake** (v3.20 or higher)
2. **Ninja** build generator (recommended)
3. A **C++23 compatible compiler** (MSVC on Windows, GCC/Clang on Linux)
4. **SDL3** development library (installed and visible via CMake config)

---

## How to Build

### Build the Zwodee Engine
Configure and build the engine library from the `zwodee` directory:
```bash
cd zwodee
cmake --preset windows-x64-debug
cmake --build out/build/windows-x64-debug
```

*Note: Build outputs will copy resources and DLLs automatically to the output directory.*

---

## Quick Start API Guide

### Initializing the Engine
```cpp
#include "zwodee.hpp"

int main() {
    // Instantiate engine (Title, Width, Height, VSync)
    zwodee::engine engine("My Zwodee Game", 800, 600, true);
    
    // Start 128Hz game loop
    engine.run();
    return 0;
}
```

### Custom Level Setup
```cpp
#include "zwodee.hpp"

class MyLevel : public zwodee::tile_level {
public:
    MyLevel() : tile_level(25, 19) {}

    void on_enter() override {
        // Load background
        auto bg = get_renderer().load_dds_texture("assets/background.dds");
        set_background_texture(bg.get());

        // Spawn a player entity
        auto player = std::make_unique<my_player_entity>(1, player_texture);
        add_entity(std::move(player));
    }

    void tick() override {
        tile_level::tick(); // Updates dynamic entities
        
        // Handle custom level rules, triggers or collision sweeps
    }
};
```

### Unified Collision Checking
```cpp
// Check if player collides with an enemy or a wall tile
if (player.collides_with(enemy)) {
    player.take_damage(10);
    player.on_collision(enemy);
}
```
