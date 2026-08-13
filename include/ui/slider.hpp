#pragma once

#include "api/api.hpp"
#include "graphics/render-snapshot.hpp"
#include "graphics/font.hpp"
#include <string>
#include <algorithm>

namespace zwodee
{
    /**
     * @class   slider
     * @brief   UI Slider component for volume control and numerical adjustments.
     */
    class ZWODEE_API slider
    {
    public:
        slider() = default;
        slider(const std::string& label, float value, float min_val, float max_val, float x, float y, float w, float h);

        bool is_hovered(float mx, float my) const;

        /**
         * @brief Handles mouse interaction (click or drag). Updates value based on mouse X coordinate.
         * @return true if the value changed.
         */
        bool handle_mouse(float mx, float my, bool is_mouse_down, bool is_mouse_click = true);
        void reset_drag() { m_is_dragging = false; }

        /**
         * @brief Adjusts value by a relative delta (e.g. +0.05f or -0.05f for keyboard control).
         */
        void adjust_value(float delta);

        void add_to_snapshot(render_snapshot& snapshot, const font& f, bool selected_or_hovered) const;

        inline float get_value()     const { return m_value; }
        inline float get_min_value() const { return m_min_val; }
        inline float get_max_value() const { return m_max_val; }

        inline void set_value(float val) { m_value = std::clamp(val, m_min_val, m_max_val); }

        float get_normalized_value() const;
        void set_normalized_value(float norm);

        bool is_enabled() const { return m_enabled; }
        void set_enabled(bool enabled) { m_enabled = enabled; }

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
        bool m_enabled = true;
        float m_value = 0.0f;
        float m_min_val = 0.0f;
        float m_max_val = 1.0f;
        float m_x = 0.0f;
        float m_y = 0.0f;
        float m_w = 0.0f;
        float m_h = 0.0f;
        bool m_is_dragging = false;
    };
}
