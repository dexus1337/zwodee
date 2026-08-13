#pragma once

#include "api/api.hpp"
#include "graphics/render-snapshot.hpp"
#include <string>
#include <memory>
#include <array>
#include <vector>

namespace zwodee
{
    class renderer;
    class texture;

    struct ZWODEE_API glyph_info
    {
        uint16_t x0 = 0, y0 = 0, x1 = 0, y1 = 0; // Coordinates in texture atlas
        float xoff = 0.0f, yoff = 0.0f, xadvance = 0.0f;
        float xoff2 = 0.0f, yoff2 = 0.0f;
    };

    /**
     * @class   font
     * @brief   Loads a TrueType font (.ttf) file and bakes it into a texture atlas for rendering.
     */
    class ZWODEE_API font
    {
    public:
        /**
         * @brief Loads a TTF font and bakes ASCII 32-127 at the specified pixel size.
         */
        font(renderer& r, const std::string& ttf_path, float font_size_px);
        ~font();

        // Prevent copying
        font(const font&) = delete;
        font& operator=(const font&) = delete;

        inline const texture* get_texture()   const { return m_atlas_texture.get(); }
        inline float          get_font_size() const { return m_font_size; }

        const glyph_info& get_glyph(char c) const;
        std::vector<render_node> get_text_nodes(const std::string& text, float x, float y, float scale = 1.0f, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255) const;

    private:
        std::unique_ptr<texture> m_atlas_texture;
        std::array<glyph_info, 96> m_glyphs{}; // Store printable ASCII characters (32 to 127)
        float m_font_size = 0.0f;
    };
}
