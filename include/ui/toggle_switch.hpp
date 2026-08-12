#pragma once

#include "api/api.hpp"
#include "graphics/render-snapshot.hpp"
#include "graphics/font.hpp"
#include <string>

namespace zwodee
{
    /**
     * @class   toggle_switch
     * @brief   UI Toggle Switch component for binary settings (e.g. Sound ON/OFF).
     */
    class ZWODEE_API toggle_switch
    {
    public:
        toggle_switch() = default;
        toggle_switch(const std::string& label, bool is_on, float x, float y, float w, float h);

        bool is_hovered(float mx, float my) const;

        bool is_on() const { return m_on; }
        void set_on(bool is_on) { m_on = is_on; }
        void toggle() { if (m_enabled) m_on = !m_on; }

        bool is_enabled() const { return m_enabled; }
        void set_enabled(bool enabled) { m_enabled = enabled; }

        void add_to_snapshot(render_snapshot& snapshot, const font& f, bool selected_or_hovered) const;

        float get_x() const { return m_x; }
        float get_y() const { return m_y; }
        float get_w() const { return m_w; }
        float get_h() const { return m_h; }
        const std::string& get_label() const { return m_label; }

        void set_label(const std::string& label) { m_label = label; }
        void set_position(float x, float y) { m_x = x; m_y = y; }
        void set_size(float w, float h) { m_w = w; m_h = h; }

    private:
        std::string m_label;
        bool m_on = true;
        bool m_enabled = true;
        float m_x = 0.0f;
        float m_y = 0.0f;
        float m_w = 0.0f;
        float m_h = 0.0f;
    };
}
