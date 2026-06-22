#include "graphics/texture.hpp"
#include <SDL3/SDL.h>

namespace zwodee
{
    texture::texture(SDL_Texture* sdl_texture, int width, int height)
        : m_sdl_texture(sdl_texture), m_width(width), m_height(height)
    {
    }

    texture::~texture()
    {
        // Guard clause: Avoid destroying null texture
        if (!m_sdl_texture)
        {
            return;
        }

        SDL_DestroyTexture(m_sdl_texture);
        m_sdl_texture = nullptr;
    }

    SDL_Texture* texture::get_raw_texture() const
    {
        return m_sdl_texture;
    }

    int texture::get_width() const
    {
        return m_width;
    }

    int texture::get_height() const
    {
        return m_height;
    }
}
