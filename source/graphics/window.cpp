#include "graphics/window.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>

namespace zwodee
{
    window::window(const std::string& title, int width, int height)
        : m_width(width), m_height(height)
    {
        // Guard clause: Initialize SDL video subsystem
        if (!SDL_WasInit(SDL_INIT_VIDEO) && !SDL_InitSubSystem(SDL_INIT_VIDEO))
        {
            throw std::runtime_error(std::string("Failed to initialize SDL Video: ") + SDL_GetError());
        }

        // Create window using SDL3 API (with resizable flag)
        m_sdl_window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);

        // Guard clause: Check window creation
        if (!m_sdl_window)
        {
            throw std::runtime_error(std::string("Failed to create SDL Window: ") + SDL_GetError());
        }
    }

    window::~window()
    {
        // Guard clause: Avoid destroying null window
        if (!m_sdl_window)
        {
            return;
        }

        SDL_DestroyWindow(m_sdl_window);
        m_sdl_window = nullptr;
    }

    SDL_Window* window::get_raw_window() const
    {
        return m_sdl_window;
    }

    float window::get_scale_factor() const
    {
        int w = 0, h = 0;
        SDL_GetWindowSize(m_sdl_window, &w, &h);
        float scale = static_cast<float>(w) / 1120.0f;
        if (scale < 1.0f) scale = 1.0f;
        return scale;
    }

    int window::get_width() const
    {
        int w = 0, h = 0;
        SDL_GetWindowSize(m_sdl_window, &w, &h);
        return static_cast<int>(static_cast<float>(w) / get_scale_factor());
    }

    int window::get_height() const
    {
        int w = 0, h = 0;
        SDL_GetWindowSize(m_sdl_window, &w, &h);
        return static_cast<int>(static_cast<float>(h) / get_scale_factor());
    }
}
