#include "graphics/font.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "graphics/external/stb_truetype.h"

#include <SDL3/SDL.h>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cmath>

namespace zwodee
{
    font::font(renderer& r, const std::string& ttf_path, float font_size_px)
        : m_font_size(font_size_px)
    {
        // 1. Read TTF file
        std::ifstream file(ttf_path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            throw std::runtime_error("Failed to open font file: " + ttf_path);
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        {
            throw std::runtime_error("Failed to read font file: " + ttf_path);
        }

        // 2. Bake characters into a temporary single-channel alpha bitmap using oversampled packing
        const int atlas_w = 1024;
        const int atlas_h = 1024;
        std::vector<uint8_t> alpha_pixels(atlas_w * atlas_h);
        
        stbtt_pack_context pc;
        if (!stbtt_PackBegin(&pc, alpha_pixels.data(), atlas_w, atlas_h, 0, 1, nullptr))
        {
            throw std::runtime_error("Failed to initialize font pack context");
        }
        
        // 2x horizontal and 2x vertical oversampling for subpixel sharpness
        stbtt_PackSetOversampling(&pc, 2, 2);
        
        std::array<stbtt_packedchar, 96> packed_chars{};
        stbtt_pack_range range{};
        range.font_size = font_size_px;
        range.first_unicode_codepoint_in_range = 32;
        range.array_of_unicode_codepoints = nullptr;
        range.num_chars = 96;
        range.chardata_for_range = packed_chars.data();
        
        if (!stbtt_PackFontRanges(&pc, buffer.data(), 0, &range, 1))
        {
            stbtt_PackEnd(&pc);
            throw std::runtime_error("Failed to pack font ranges for: " + ttf_path);
        }
        stbtt_PackEnd(&pc);

        // 3. Convert single-channel alpha map to 4-channel RGBA map
        std::vector<uint8_t> rgba_pixels(atlas_w * atlas_h * 4);
        for (int i = 0; i < atlas_w * atlas_h; ++i)
        {
            uint8_t alpha = alpha_pixels[i];
            rgba_pixels[i * 4 + 0] = 255;   // R
            rgba_pixels[i * 4 + 1] = 255;   // G
            rgba_pixels[i * 4 + 2] = 255;   // B
            rgba_pixels[i * 4 + 3] = alpha; // A
        }

        SDL_Renderer* raw_renderer = r.get_raw_renderer();
        SDL_Texture* sdl_tex = SDL_CreateTexture(raw_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, atlas_w, atlas_h);
        if (!sdl_tex)
        {
            throw std::runtime_error(std::string("Failed to create font texture: ") + SDL_GetError());
        }

        if (!SDL_UpdateTexture(sdl_tex, nullptr, rgba_pixels.data(), atlas_w * 4))
        {
            SDL_DestroyTexture(sdl_tex);
            throw std::runtime_error(std::string("Failed to update font texture: ") + SDL_GetError());
        }

        SDL_SetTextureBlendMode(sdl_tex, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(sdl_tex, SDL_SCALEMODE_LINEAR);

        m_atlas_texture = std::make_unique<texture>(sdl_tex, atlas_w, atlas_h);

        // 4. Copy metrics
        for (size_t i = 0; i < 96; ++i)
        {
            m_glyphs[i].x0 = packed_chars[i].x0;
            m_glyphs[i].y0 = packed_chars[i].y0;
            m_glyphs[i].x1 = packed_chars[i].x1;
            m_glyphs[i].y1 = packed_chars[i].y1;
            m_glyphs[i].xoff = packed_chars[i].xoff;
            m_glyphs[i].yoff = packed_chars[i].yoff;
            m_glyphs[i].xadvance = packed_chars[i].xadvance;
            m_glyphs[i].xoff2 = packed_chars[i].xoff2;
            m_glyphs[i].yoff2 = packed_chars[i].yoff2;
        }
    }

    font::~font() = default;

    const texture* font::get_texture() const
    {
        return m_atlas_texture.get();
    }

    const glyph_info& font::get_glyph(char c) const
    {
        int idx = static_cast<int>(c) - 32;
        if (idx < 0 || idx >= 96)
        {
            // Return space (index 0) on out of bounds
            return m_glyphs[0];
        }
        return m_glyphs[idx];
    }

    float font::get_font_size() const
    {
        return m_font_size;
    }

    std::vector<render_node> font::get_text_nodes(const std::string& text, float x, float y, float scale, uint8_t r, uint8_t g, uint8_t b, uint8_t a) const
    {
        std::vector<render_node> nodes;
        if (!m_atlas_texture)
        {
            return nodes;
        }

        float current_x = std::round(x);
        float current_y = std::round(y);

        for (char c : text)
        {
            if (c == '\n')
            {
                current_x = std::round(x);
                current_y += m_font_size * scale;
                continue;
            }

            const glyph_info& glyph = get_glyph(c);

            float src_x = static_cast<float>(glyph.x0);
            float src_y = static_cast<float>(glyph.y0);
            float src_w = static_cast<float>(glyph.x1 - glyph.x0);
            float src_h = static_cast<float>(glyph.y1 - glyph.y0);

            float dest_x = current_x + glyph.xoff * scale;
            float dest_y = current_y + glyph.yoff * scale;
            float dest_w = (glyph.xoff2 - glyph.xoff) * scale;
            float dest_h = (glyph.yoff2 - glyph.yoff) * scale;

            render_node node{};
            node.x = dest_x;
            node.y = dest_y;
            node.w = dest_w;
            node.h = dest_h;
            node.tex = m_atlas_texture.get();
            node.src_x = static_cast<int>(src_x);
            node.src_y = static_cast<int>(src_y);
            node.src_w = static_cast<int>(src_w);
            node.src_h = static_cast<int>(src_h);
            node.r = r;
            node.g = g;
            node.b = b;
            node.a = a;

            nodes.push_back(node);

            current_x += glyph.xadvance * scale;
        }

        return nodes;
    }
}
