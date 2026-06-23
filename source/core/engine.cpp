#include "core/engine.hpp"
#include "graphics/window.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"
#include "audio/audio-manager.hpp"
#include "levels/level-manager.hpp"
#include "core/input.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>

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

        // Render loop (runs on Main thread)
        while (m_running)
        {
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
                for (const auto& node : *snapshot)
                {
                    if (node.tex)
                    {
                        SDL_SetTextureColorMod(node.tex->get_raw_texture(), node.color_mod, node.color_mod, node.color_mod);
                        m_renderer->draw_sprite(*node.tex, node.src_x, node.src_y, node.src_w, node.src_h, node.x, node.y, node.w, node.h, node.flip_horizontal, node.flip_vertical);
                        SDL_SetTextureColorMod(node.tex->get_raw_texture(), 255, 255, 255);
                    }
                    else
                    {
                        // Draw red placeholder square for untextured entities
                        SDL_Renderer* sdl_renderer = m_renderer->get_raw_renderer();
                        SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
                        SDL_FRect rect = { node.x, node.y, node.w, node.h };
                        SDL_RenderFillRect(sdl_renderer, &rect);
                    }
                }
            }
            
            m_renderer->present();

            // Give CPU some breathing room if vsync is off
            SDL_Delay(1);
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
                    mask = input_state::quit;
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
}
