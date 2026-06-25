#pragma once

/**
 * @file    engine.hpp
 * @author  dexus1337
 * @brief   Defines the central engine class, controlling SDL3 lifecycle and game loops.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "graphics/render-snapshot.hpp"
#include "core/input.hpp"

struct SDL_Texture;

namespace zwodee
{
    class window;
    class renderer;
    class audio_manager;
    class level_manager;
    struct input_state;

    /**
     * @class   engine
     * @brief   Core engine class managing loops, timing, systems, and cleanup.
     */
    class ZWODEE_API engine
    {
    public:
        /**
         * @brief Constructs the engine with window options.
         */
        engine(const std::string& title, int width, int height, bool vsync = true);

        /**
         * @brief Destroys the engine and cleanly shuts down SDL3.
         */
        ~engine();

        // Prevent copying
        engine(const engine&) = delete;
        engine& operator=(const engine&) = delete;

        /**
         * @brief Begins execution of the engine's 128Hz game loop.
         */
        void run();

        /**
         * @brief Stops the engine loop.
         */
        void stop();

        // System accessors
        [[nodiscard]] window& get_window();
        [[nodiscard]] renderer& get_renderer();
        [[nodiscard]] audio_manager& get_audio_manager();
        [[nodiscard]] level_manager& get_level_manager();

        /**
         * @brief Gets current simulated tick index.
         */
        [[nodiscard]] uint64_t get_current_tick() const;

        enum class fps_limit
        {
            fps_60,
            fps_144,
            fps_240,
            fps_360,
            fps_480,
            unlocked,
            vsync
        };

        void set_fps_limit(fps_limit limit);
        [[nodiscard]] fps_limit get_fps_limit() const;

    private:
        /**
         * @brief Runs the 128Hz simulation thread logic.
         */
        void run_simulation();

        /**
         * @brief Polls and processes pending SDL events.
         */
        void process_events(input_state& current_input);

        std::atomic<bool> m_running = false;
        std::atomic<uint64_t> m_current_tick = 0;
        std::atomic<fps_limit> m_fps_limit{fps_limit::vsync};

        // Multithreading sync structures
        std::thread m_simulation_thread;
        mutable std::mutex m_input_mutex;
        
        // Input synchronization
        input_state m_shared_input;

        // Double-buffered frame snapshot (atomic swap pointer)
        std::atomic<render_snapshot*> m_render_frontbuffer{nullptr};
        std::atomic<render_snapshot*> m_render_backbuffer{nullptr};

        std::unique_ptr<window> m_window;
        std::unique_ptr<renderer> m_renderer;
        std::unique_ptr<audio_manager> m_audio_manager;
        std::unique_ptr<level_manager> m_level_manager;
        struct SDL_Texture* m_blur_target = nullptr;
        int m_blur_w = 0;
        int m_blur_h = 0;
    };
}
