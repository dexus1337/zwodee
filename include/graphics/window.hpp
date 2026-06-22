#pragma once

/**
 * @file    window.hpp
 * @author  dexus1337
 * @brief   Defines the window class wrapping SDL_Window.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"
#include <string>

// Forward declaration of SDL structures
struct SDL_Window;

namespace zwodee
{
    /**
     * @class   window
     * @brief   An OOP wrapper for the SDL window.
     */
    class ZWODEE_API window
    {
    public:
        /**
         * @brief Constructs a new window object.
         */
        window(const std::string& title, int width, int height);

        /**
         * @brief Destroys the window and releases SDL resources.
         */
        ~window();

        // Prevent copying
        window(const window&) = delete;
        window& operator=(const window&) = delete;

        /**
         * @brief Gets the raw SDL_Window pointer.
         */
        [[nodiscard]] SDL_Window* get_raw_window() const;

        /**
         * @brief Gets dynamic pixel-art scale factor based on window height.
         */
        [[nodiscard]] float get_scale_factor() const;

        /**
         * @brief Gets window width in pixels.
         */
        [[nodiscard]] int get_width() const;

        /**
         * @brief Gets window height in pixels.
         */
        [[nodiscard]] int get_height() const;

    private:
        SDL_Window* m_sdl_window = nullptr;
        int m_width = 0;
        int m_height = 0;
    };
}
