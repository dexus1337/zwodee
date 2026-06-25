#include "graphics/renderer.hpp"
#include "graphics/window.hpp"
#include "graphics/texture.hpp"
#include "graphics/dds-format.hpp"
#include "graphics/font.hpp"

#include <SDL3/SDL.h>
#include <stdexcept>
#include <fstream>
#include <vector>

namespace zwodee
{


    // ==========================================
    // Renderer Implementation
    // ==========================================

    renderer::renderer(const window& target_window, bool vsync)
    {
        // Create renderer using default driver
        m_sdl_renderer = SDL_CreateRenderer(target_window.get_raw_window(), nullptr);

        // Guard clause: Check renderer creation
        if (!m_sdl_renderer)
        {
            throw std::runtime_error(std::string("Failed to create SDL Renderer: ") + SDL_GetError());
        }

        // Set VSync mode
        set_vsync(vsync);
    }

    renderer::~renderer()
    {
        // Guard clause: Avoid destroying null renderer
        if (!m_sdl_renderer)
        {
            return;
        }

        SDL_DestroyRenderer(m_sdl_renderer);
        m_sdl_renderer = nullptr;
    }

    void renderer::clear()
    {
        SDL_RenderClear(m_sdl_renderer);
    }

    void renderer::present()
    {
        SDL_RenderPresent(m_sdl_renderer);
    }

    void renderer::set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_SetRenderDrawColor(m_sdl_renderer, r, g, b, a);
    }

    void renderer::set_vsync(bool enable)
    {
        // SDL3 uses SDL_SetRenderVSync
        SDL_SetRenderVSync(m_sdl_renderer, enable ? 1 : 0);
    }

    std::unique_ptr<texture> renderer::load_texture(const std::string& file_path)
    {
        // Load surface
        SDL_Surface* surface = SDL_LoadBMP(file_path.c_str());

        // Guard clause: Check surface loading
        if (!surface)
        {
            return nullptr;
        }

        int width = surface->w;
        int height = surface->h;

        // Create texture from surface
        SDL_Texture* tex = SDL_CreateTextureFromSurface(m_sdl_renderer, surface);
        SDL_DestroySurface(surface); // Clean up temp surface

        // Guard clause: Check texture creation
        if (!tex)
        {
            return nullptr;
        }

        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_LINEAR);

        return std::make_unique<texture>(tex, width, height);
    }

    #include "graphics/dds-format.hpp"

    std::unique_ptr<texture> renderer::load_dds_texture(const std::string& file_path)
    {
        std::ifstream file(file_path, std::ios::binary);
        if (!file)
        {
            return nullptr;
        }

        // Read header
        dds_header header{};
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (file.gcount() != sizeof(header) || header.magic != 0x20534444) // "DDS "
        {
            return nullptr;
        }

        int width = static_cast<int>(header.width);
        int height = static_cast<int>(header.height);

        // Read pixel data
        std::vector<uint8_t> pixels(width * height * 4);
        file.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
        if (file.gcount() != static_cast<std::streamsize>(pixels.size()))
        {
            return nullptr;
        }

        // Create streaming texture in SDL3
        SDL_Texture* tex = SDL_CreateTexture(m_sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
        if (!tex)
        {
            return nullptr;
        }

        // Upload texture pixels to GPU
        if (!SDL_UpdateTexture(tex, nullptr, pixels.data(), width * 4))
        {
            SDL_DestroyTexture(tex);
            return nullptr;
        }

        // Set blend mode to enable transparency
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_LINEAR);

        return std::make_unique<texture>(tex, width, height);
    }

    void renderer::draw_texture(const texture& tex, float x, float y)
    {
        SDL_FRect dest_rect = { x, y, static_cast<float>(tex.get_width()), static_cast<float>(tex.get_height()) };

        // SDL3 uses SDL_RenderTexture
        SDL_RenderTexture(m_sdl_renderer, tex.get_raw_texture(), nullptr, &dest_rect);
    }

    void renderer::draw_sprite(const texture& tex, int src_x, int src_y, int src_w, int src_h, float dest_x, float dest_y, float dest_w, float dest_h, bool flip_horizontal, bool flip_vertical)
    {
        SDL_FRect src_rect = { static_cast<float>(src_x), static_cast<float>(src_y), static_cast<float>(src_w), static_cast<float>(src_h) };
        SDL_FRect dest_rect = { dest_x, dest_y, dest_w, dest_h };

        if (flip_horizontal || flip_vertical)
        {
            SDL_FlipMode flip = SDL_FLIP_NONE;
            if (flip_horizontal) flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_HORIZONTAL);
            if (flip_vertical) flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_VERTICAL);
            SDL_RenderTextureRotated(m_sdl_renderer, tex.get_raw_texture(), &src_rect, &dest_rect, 0.0, nullptr, flip);
        }
        else
        {
            SDL_RenderTexture(m_sdl_renderer, tex.get_raw_texture(), &src_rect, &dest_rect);
        }
    }

    SDL_Renderer* renderer::get_raw_renderer() const
    {
        return m_sdl_renderer;
    }

    void renderer::draw_text(const font& f, const std::string& text, float x, float y, float scale, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        const texture* font_tex = f.get_texture();
        if (!font_tex)
        {
            return;
        }

        SDL_Texture* raw_tex = font_tex->get_raw_texture();
        if (!raw_tex)
        {
            return;
        }

        // Apply color and alpha modulation
        SDL_SetTextureColorMod(raw_tex, r, g, b);
        SDL_SetTextureAlphaMod(raw_tex, a);

        float current_x = x;
        float current_y = y;

        for (char c : text)
        {
            if (c == '\n')
            {
                current_x = x;
                current_y += f.get_font_size() * scale;
                continue;
            }

            const glyph_info& glyph = f.get_glyph(c);

            // Source rect from font atlas
            float src_x = static_cast<float>(glyph.x0);
            float src_y = static_cast<float>(glyph.y0);
            float src_w = static_cast<float>(glyph.x1 - glyph.x0);
            float src_h = static_cast<float>(glyph.y1 - glyph.y0);

            // Dest rect on screen
            float dest_x = current_x + glyph.xoff * scale;
            float dest_y = current_y + glyph.yoff * scale;
            float dest_w = (glyph.xoff2 - glyph.xoff) * scale;
            float dest_h = (glyph.yoff2 - glyph.yoff) * scale;

            SDL_FRect src_rect = { src_x, src_y, src_w, src_h };
            SDL_FRect dest_rect = { dest_x, dest_y, dest_w, dest_h };

            SDL_RenderTexture(m_sdl_renderer, raw_tex, &src_rect, &dest_rect);

            current_x += glyph.xadvance * scale;
        }

        // Reset texture color/alpha modulation back to default white/fully opaque
        SDL_SetTextureColorMod(raw_tex, 255, 255, 255);
        SDL_SetTextureAlphaMod(raw_tex, 255);
    }
}
