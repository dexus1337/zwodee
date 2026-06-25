#include "core/engine.hpp"
#include "graphics/window.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"
#include "audio/audio-manager.hpp"
#include "levels/level-manager.hpp"
#include "core/input.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>
#include <chrono>

namespace zwodee
{
    engine::engine(const std::string& title, int width, int height, bool vsync)
    {
        // Guard clause: Initialize SDL core systems
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
        }

        m_window = std::make_unique<window>(title, width, height);
        m_renderer = std::make_unique<renderer>(*m_window, vsync);
        m_audio_manager = std::make_unique<audio_manager>();
        m_level_manager = std::make_unique<level_manager>();

        // Allocate double buffer structures
        m_render_frontbuffer.store(new render_snapshot());
        m_render_backbuffer.store(new render_snapshot());
    }

    engine::~engine()
    {
        stop();
        if (m_simulation_thread.joinable())
        {
            m_simulation_thread.join();
        }

        // Delete snapshot buffers
        delete m_render_frontbuffer.load();
        delete m_render_backbuffer.load();

        m_level_manager.reset();
        m_audio_manager.reset();
        
        if (m_blur_target)
        {
            SDL_DestroyTexture(m_blur_target);
            m_blur_target = nullptr;
        }

        m_renderer.reset();
        m_window.reset();
        SDL_Quit();
    }

    void engine::run()
    {
        m_running = true;

        // Start the physics simulation loop on the second thread
        m_simulation_thread = std::thread(&engine::run_simulation, this);

        input_state current_input;
        fps_limit current_limit = m_fps_limit;

        // Sync initial vsync state
        m_renderer->set_vsync(current_limit == fps_limit::vsync);

        // Render loop (runs on Main thread)
        while (m_running)
        {
            uint64_t frame_start = SDL_GetTicksNS();

            // Handle FPS/VSync limit changes dynamically
            if (m_fps_limit != current_limit)
            {
                current_limit = m_fps_limit;
                m_renderer->set_vsync(current_limit == fps_limit::vsync);
            }

            process_events(current_input);

            // Guard clause: Exit if quit is requested
            if (current_input.is_down(input_state::quit))
            {
                m_running = false;
                break;
            }

            // Lock-free input update
            {
                std::lock_guard<std::mutex> lock(m_input_mutex);
                m_shared_input = current_input;
            }

            // Perform rendering lock-free
            float scale = m_window->get_scale_factor();
            SDL_SetRenderScale(m_renderer->get_raw_renderer(), scale, scale);

            m_renderer->clear();
            
            // Draw all nodes in the frontbuffer
            render_snapshot* snapshot = m_render_frontbuffer.load();
            if (snapshot)
            {
                SDL_Renderer* sdl_renderer = m_renderer->get_raw_renderer();
                bool has_blur = false;
                for (const auto& node : *snapshot)
                {
                    if (node.is_blur)
                    {
                        has_blur = true;
                        break;
                    }
                }

                if (has_blur)
                {

                    // Create or recreate the blur target dynamically to match the current logical dimensions / 2
                    int blur_w = m_window->get_width() / 2;
                    int blur_h = m_window->get_height() / 2;
                    if (blur_w < 1) blur_w = 1;
                    if (blur_h < 1) blur_h = 1;

                    if (m_blur_target && (m_blur_w != blur_w || m_blur_h != blur_h))
                    {
                        SDL_DestroyTexture(m_blur_target);
                        m_blur_target = nullptr;
                        m_blur_w = 0;
                        m_blur_h = 0;
                    }

                    if (!m_blur_target)
                    {
                        m_blur_target = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, blur_w, blur_h);
                        if (m_blur_target)
                        {
                            SDL_SetTextureBlendMode(m_blur_target, SDL_BLENDMODE_BLEND);
                            SDL_SetTextureScaleMode(m_blur_target, SDL_SCALEMODE_LINEAR);
                            m_blur_w = blur_w;
                            m_blur_h = blur_h;
                        }
                    }

                    if (m_blur_target)
                    {
                        // 1. Draw non-UI, non-blur (gameplay background) elements to the low-res blur target
                        SDL_SetRenderTarget(sdl_renderer, m_blur_target);
                        SDL_SetRenderDrawColor(sdl_renderer, 15, 15, 25, 255); // Clear color
                        SDL_RenderClear(sdl_renderer);

                        // Scale drawing down by 0.5 to fit the texture
                        SDL_SetRenderScale(sdl_renderer, 0.5f, 0.5f);

                        for (const auto& node : *snapshot)
                        {
                            if (!node.is_ui && !node.is_blur)
                            {
                                if (node.tex)
                                {
                                    uint8_t final_r = (node.color_mod != 255) ? node.color_mod : node.r;
                                    uint8_t final_g = (node.color_mod != 255) ? node.color_mod : node.g;
                                    uint8_t final_b = (node.color_mod != 255) ? node.color_mod : node.b;
                                    SDL_SetTextureColorMod(node.tex->get_raw_texture(), final_r, final_g, final_b);
                                    SDL_SetTextureAlphaMod(node.tex->get_raw_texture(), node.a);
                                    m_renderer->draw_sprite(*node.tex, node.src_x, node.src_y, node.src_w, node.src_h, node.x, node.y, node.w, node.h, node.flip_horizontal, node.flip_vertical);
                                    SDL_SetTextureColorMod(node.tex->get_raw_texture(), 255, 255, 255);
                                    SDL_SetTextureAlphaMod(node.tex->get_raw_texture(), 255);
                                }
                                else
                                {
                                    SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
                                    SDL_SetRenderDrawColor(sdl_renderer, node.r, node.g, node.b, node.a);
                                    SDL_FRect rect = { node.x, node.y, node.w, node.h };
                                    SDL_RenderFillRect(sdl_renderer, &rect);
                                }
                            }
                        }

                        // 2. Restore rendering target to the main screen backbuffer
                        SDL_SetRenderTarget(sdl_renderer, nullptr);

                        // Restore main screen render scale
                        float main_scale = m_window->get_scale_factor();
                        SDL_SetRenderScale(sdl_renderer, main_scale, main_scale);
                    }

                    // 3. Draw background elements normally to the main screen
                    for (const auto& node : *snapshot)
                    {
                        if (!node.is_ui && !node.is_blur)
                        {
                            if (node.tex)
                            {
                                uint8_t final_r = (node.color_mod != 255) ? node.color_mod : node.r;
                                uint8_t final_g = (node.color_mod != 255) ? node.color_mod : node.g;
                                uint8_t final_b = (node.color_mod != 255) ? node.color_mod : node.b;
                                SDL_SetTextureColorMod(node.tex->get_raw_texture(), final_r, final_g, final_b);
                                SDL_SetTextureAlphaMod(node.tex->get_raw_texture(), node.a);
                                m_renderer->draw_sprite(*node.tex, node.src_x, node.src_y, node.src_w, node.src_h, node.x, node.y, node.w, node.h, node.flip_horizontal, node.flip_vertical);
                                SDL_SetTextureColorMod(node.tex->get_raw_texture(), 255, 255, 255);
                                SDL_SetTextureAlphaMod(node.tex->get_raw_texture(), 255);
                            }
                            else
                            {
                                SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
                                SDL_SetRenderDrawColor(sdl_renderer, node.r, node.g, node.b, node.a);
                                SDL_FRect rect = { node.x, node.y, node.w, node.h };
                                SDL_RenderFillRect(sdl_renderer, &rect);
                            }
                        }
                    }

                    // 4. Draw the blur nodes (9-tap Gaussian GPU blur backdrop + overlay color/opacity)
                    for (const auto& node : *snapshot)
                    {
                        if (node.is_blur)
                        {
                            SDL_FRect dest_rect = { node.x, node.y, node.w, node.h };
                            if (m_blur_target)
                            {
                                // Define tap offsets and weights (accumulating to 1.0)
                                struct Tap { float dx; float dy; uint8_t alpha; };
                                float offset = 2.0f; // Blur spread radius
                                Tap taps[] = {
                                    { 0.0f, 0.0f, 64 },          // Center (weight: 0.25)
                                    { -offset, 0.0f, 32 },       // Orthogonals (weight: 0.125 each)
                                    { offset, 0.0f, 32 },
                                    { 0.0f, -offset, 32 },
                                    { 0.0f, offset, 32 },
                                    { -offset, -offset, 16 },    // Diagonals (weight: 0.0625 each)
                                    { offset, -offset, 16 },
                                    { -offset, offset, 16 },
                                    { offset, offset, 16 }
                                };

                                for (const auto& tap : taps)
                                {
                                    SDL_FRect tap_dest = { node.x + tap.dx, node.y + tap.dy, node.w, node.h };
                                    SDL_FRect src_rect = { node.x * 0.5f, node.y * 0.5f, node.w * 0.5f, node.h * 0.5f };
                                    SDL_SetTextureAlphaMod(m_blur_target, tap.alpha);
                                    SDL_RenderTexture(sdl_renderer, m_blur_target, &src_rect, &tap_dest);
                                }
                                SDL_SetTextureAlphaMod(m_blur_target, 255); // Reset
                            }
                            
                            // Draw the color overlay
                            SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
                            SDL_SetRenderDrawColor(sdl_renderer, node.r, node.g, node.b, node.a);
                            SDL_RenderFillRect(sdl_renderer, &dest_rect);
                        }
                    }

                    // 5. Draw the UI nodes directly to the screen (unblurred and crisp!)
                    for (const auto& node : *snapshot)
                    {
                        if (node.is_ui && !node.is_blur)
                        {
                            if (node.tex)
                            {
                                uint8_t final_r = (node.color_mod != 255) ? node.color_mod : node.r;
                                uint8_t final_g = (node.color_mod != 255) ? node.color_mod : node.g;
                                uint8_t final_b = (node.color_mod != 255) ? node.color_mod : node.b;
                                SDL_SetTextureColorMod(node.tex->get_raw_texture(), final_r, final_g, final_b);
                                SDL_SetTextureAlphaMod(node.tex->get_raw_texture(), node.a);
                                m_renderer->draw_sprite(*node.tex, node.src_x, node.src_y, node.src_w, node.src_h, node.x, node.y, node.w, node.h, node.flip_horizontal, node.flip_vertical);
                                SDL_SetTextureColorMod(node.tex->get_raw_texture(), 255, 255, 255);
                                SDL_SetTextureAlphaMod(node.tex->get_raw_texture(), 255);
                            }
                            else
                            {
                                SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
                                SDL_SetRenderDrawColor(sdl_renderer, node.r, node.g, node.b, node.a);
                                SDL_FRect rect = { node.x, node.y, node.w, node.h };
                                SDL_RenderFillRect(sdl_renderer, &rect);
                            }
                        }
                    }
                }
                else
                {
                    // Render normally (no blur needed)
                    SDL_SetRenderScale(sdl_renderer, scale, scale);
                    for (const auto& node : *snapshot)
                    {
                        if (node.tex)
                        {
                            uint8_t final_r = (node.color_mod != 255) ? node.color_mod : node.r;
                            uint8_t final_g = (node.color_mod != 255) ? node.color_mod : node.g;
                            uint8_t final_b = (node.color_mod != 255) ? node.color_mod : node.b;
                            SDL_SetTextureColorMod(node.tex->get_raw_texture(), final_r, final_g, final_b);
                            SDL_SetTextureAlphaMod(node.tex->get_raw_texture(), node.a);
                            m_renderer->draw_sprite(*node.tex, node.src_x, node.src_y, node.src_w, node.src_h, node.x, node.y, node.w, node.h, node.flip_horizontal, node.flip_vertical);
                            SDL_SetTextureColorMod(node.tex->get_raw_texture(), 255, 255, 255);
                            SDL_SetTextureAlphaMod(node.tex->get_raw_texture(), 255);
                        }
                        else
                        {
                            SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
                            SDL_SetRenderDrawColor(sdl_renderer, node.r, node.g, node.b, node.a);
                            SDL_FRect rect = { node.x, node.y, node.w, node.h };
                            SDL_RenderFillRect(sdl_renderer, &rect);
                        }
                    }
                }
            }
            
            m_renderer->present();

            // High-precision frame rate limiting
            if (current_limit != fps_limit::vsync && current_limit != fps_limit::unlocked)
            {
                uint64_t target_ns = 1000000000ULL / 60;
                switch (current_limit)
                {
                    case fps_limit::fps_60:  target_ns = 1000000000ULL / 60; break;
                    case fps_limit::fps_144: target_ns = 1000000000ULL / 144; break;
                    case fps_limit::fps_240: target_ns = 1000000000ULL / 240; break;
                    case fps_limit::fps_360: target_ns = 1000000000ULL / 360; break;
                    case fps_limit::fps_480: target_ns = 1000000000ULL / 480; break;
                    default: break;
                }

                uint64_t frame_end = SDL_GetTicksNS();
                uint64_t elapsed = frame_end - frame_start;
                if (elapsed < target_ns)
                {
                    uint64_t sleep_ns = target_ns - elapsed;
                    // Sleep if duration is long enough to yield thread (> 2ms)
                    if (sleep_ns > 2000000ULL)
                    {
                        std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_ns - 1500000ULL)); // leave 1.5ms for busy wait
                    }
                    // Busy-wait remainder for high precision
                    while (SDL_GetTicksNS() - frame_start < target_ns)
                    {
#if defined(_MSC_VER)
                        __nop();
#else
                        asm volatile("nop");
#endif
                    }
                }
            }
            else if (current_limit == fps_limit::unlocked)
            {
                // Yield 0.1ms to prevent pure thread lockup
                std::this_thread::sleep_for(std::chrono::nanoseconds(100000ULL));
            }
            else
            {
                // VSync handles pacing, but yield a tiny bit
                std::this_thread::sleep_for(std::chrono::nanoseconds(100000ULL));
            }
        }
    }

    void engine::run_simulation()
    {
        constexpr double time_step = 1.0 / 128.0; // 128Hz Tick rate
        uint64_t last_time = SDL_GetTicks();
        double accumulator = 0.0;
        input_state local_input;

        while (m_running)
        {
            uint64_t current_time = SDL_GetTicks();
            double elapsed = (current_time - last_time) / 1000.0;
            last_time = current_time;

            if (elapsed > 0.25)
            {
                elapsed = 0.25;
            }

            accumulator += elapsed;

            bool ticked = false;

            while (accumulator >= time_step)
            {
                // Synchronize shared inputs
                {
                    std::lock_guard<std::mutex> lock(m_input_mutex);
                    local_input = m_shared_input;
                }

                // Run updates
                m_level_manager->set_player_input(local_input);
                m_level_manager->tick();

                m_current_tick++;
                accumulator -= time_step;
                ticked = true;
            }

            // If we ticked, update the backbuffer and swap it to the frontbuffer lock-free
            if (ticked)
            {
                render_snapshot* back = m_render_backbuffer.load();
                *back = m_level_manager->get_active_level_snapshot(m_window->get_width(), m_window->get_height());

                // Swap front and back pointers atomically
                render_snapshot* front = m_render_frontbuffer.exchange(back);
                m_render_backbuffer.store(front);
            }

            // Cap tick rate accuracy roughly
            SDL_Delay(1);
        }
    }

    void engine::stop()
    {
        m_running = false;
    }

    window& engine::get_window()
    {
        return *m_window;
    }

    renderer& engine::get_renderer()
    {
        return *m_renderer;
    }

    audio_manager& engine::get_audio_manager()
    {
        return *m_audio_manager;
    }

    level_manager& engine::get_level_manager()
    {
        return *m_level_manager;
    }

    uint64_t engine::get_current_tick() const
    {
        return m_current_tick;
    }

    void engine::process_events(input_state& current_input)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // Guard clause: Quit event
            if (event.type == SDL_EVENT_QUIT)
            {
                current_input.buttons |= input_state::quit;
                continue;
            }

            // Guard clause: Ignore non-key events
            if (event.type != SDL_EVENT_KEY_DOWN && event.type != SDL_EVENT_KEY_UP)
            {
                continue;
            }

            bool is_down = (event.type == SDL_EVENT_KEY_DOWN);
            uint32_t mask = 0;

            switch (event.key.key)
            {
                case SDLK_ESCAPE:
                    mask = input_state::action_2;
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    if (is_down && (event.key.mod & SDL_KMOD_ALT))
                    {
                        // Toggle fullscreen using SDL3 API
                        uint32_t flags = SDL_GetWindowFlags(m_window->get_raw_window());
                        if (flags & SDL_WINDOW_FULLSCREEN)
                        {
                            SDL_SetWindowFullscreen(m_window->get_raw_window(), false);
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(m_window->get_raw_window(), true);
                        }
                    }
                    break;
                case SDLK_UP:
                case SDLK_W:
                    mask = input_state::move_up;
                    break;
                case SDLK_DOWN:
                case SDLK_S:
                    mask = input_state::move_down;
                    break;
                case SDLK_LEFT:
                case SDLK_A:
                    mask = input_state::move_left;
                    break;
                case SDLK_RIGHT:
                case SDLK_D:
                    mask = input_state::move_right;
                    break;
                case SDLK_SPACE:
                    mask = input_state::action_1;
                    break;
                case SDLK_LCTRL:
                    mask = input_state::action_2;
                    break;
                default:
                    break;
            }

            if (is_down)
            {
                current_input.buttons |= mask;
            }
            else
            {
                current_input.buttons &= ~mask;
            }
        }
    }

    void engine::set_fps_limit(fps_limit limit)
    {
        m_fps_limit = limit;
    }

    engine::fps_limit engine::get_fps_limit() const
    {
        return m_fps_limit;
    }
}
