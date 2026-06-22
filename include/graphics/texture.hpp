#pragma once

/**
 * @file    texture.hpp
 * @author  dexus1337
 * @brief   Defines the texture wrapper class around SDL_Texture.
 * @version 1.0
 * @date    22.06.2026
 */

#include "api/api.hpp"

// Forward declaration
struct SDL_Texture;

namespace zwodee
{
    /**
     * @class   texture
     * @brief   An OOP wrapper around an SDL_Texture.
     */
    class ZWODEE_API texture
    {
    public:
        /**
         * @brief Constructs a texture from an SDL_Texture pointer.
         */
        explicit texture(SDL_Texture* sdl_texture, int width, int height);

        /**
         * @brief Destroys the texture and frees SDL resources.
         */
        ~texture();

        // Prevent copying
        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;

        [[nodiscard]] SDL_Texture* get_raw_texture() const;
        [[nodiscard]] int get_width() const;
        [[nodiscard]] int get_height() const;

    private:
        SDL_Texture* m_sdl_texture = nullptr;
        int m_width = 0;
        int m_height = 0;
    };
}
