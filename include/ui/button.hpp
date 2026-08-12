#pragma once

#include "api/api.hpp"
#include "graphics/render-snapshot.hpp"
#include "graphics/font.hpp"
#include <string>

namespace zwodee
{
    /**
     * @class   button
     * @brief   UI Button component for engine applications and menus.
     */
    class ZWODEE_API button
    {
    public:
        button() = default;
        button(const std::string& text, float x, float y, float w, float h);

        bool is_hovered(float mx, float my) const;
        void add_to_snapshot(render_snapshot& snapshot, const font& f, bool selected_or_hovered) const;

        bool is_enabled() const { return m_enabled; }
        void set_enabled(bool enabled) { m_enabled = enabled; }

        float get_x() const { return m_x; }
        float get_y() const { return m_y; }
        float get_w() const { return m_w; }
        float get_h() const { return m_h; }
        const std::string& get_text() const { return m_text; }

        void set_text(const std::string& text) { m_text = text; }
        void set_position(float x, float y) { m_x = x; m_y = y; }
        void set_size(float w, float h) { m_w = w; m_h = h; }

    private:
        std::string m_text;
        bool m_enabled = true;
        float m_x = 0.0f;
        float m_y = 0.0f;
        float m_w = 0.0f;
        float m_h = 0.0f;
    };
}
