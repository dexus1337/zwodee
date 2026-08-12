#include "ui/slider.hpp"
#include <cmath>

namespace zwodee
{
    slider::slider(const std::string& label, float value, float min_val, float max_val, float x, float y, float w, float h)
        : m_label(label), m_value(value), m_min_val(min_val), m_max_val(max_val), m_x(x), m_y(y), m_w(w), m_h(h)
    {
        set_value(value);
    }

    bool slider::is_hovered(float mx, float my) const
    {
        if (!m_enabled) return false;
        return mx >= m_x && mx <= m_x + m_w && my >= m_y && my <= m_y + m_h;
    }

    void slider::set_value(float val)
    {
        m_value = std::clamp(val, m_min_val, m_max_val);
    }

    float slider::get_normalized_value() const
    {
        if (m_max_val <= m_min_val) return 0.0f;
        return std::clamp((m_value - m_min_val) / (m_max_val - m_min_val), 0.0f, 1.0f);
    }

    void slider::set_normalized_value(float norm)
    {
        norm = std::clamp(norm, 0.0f, 1.0f);
        set_value(m_min_val + norm * (m_max_val - m_min_val));
    }

    void slider::adjust_value(float delta)
    {
        if (!m_enabled) return;
        float range = m_max_val - m_min_val;
        set_value(m_value + delta * range);
    }

    bool slider::handle_mouse(float mx, float my, bool is_mouse_down, bool is_mouse_click)
    {
        if (!m_enabled)
        {
            m_is_dragging = false;
            return false;
        }

        if (is_mouse_down)
        {
            if (m_is_dragging || (is_mouse_click && is_hovered(mx, my)))
            {
                m_is_dragging = true;
                float track_padding = 16.0f;
                float track_x = m_x + track_padding;
                float track_w = m_w - (track_padding * 2.0f);
                if (track_w < 1.0f) track_w = 1.0f;

                float rel_x = std::clamp(mx - track_x, 0.0f, track_w);
                float norm = rel_x / track_w;

                float old_value = m_value;
                set_normalized_value(norm);
                return m_value != old_value;
            }
        }
        else
        {
            m_is_dragging = false;
        }
        return false;
    }

    void slider::add_to_snapshot(render_snapshot& snapshot, const font& f, bool selected_or_hovered) const
    {
        auto add_rect = [&](float rx, float ry, float rw, float rh, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            render_node rect_node{};
            rect_node.x = rx;
            rect_node.y = ry;
            rect_node.w = rw;
            rect_node.h = rh;
            rect_node.tex = nullptr;
            rect_node.is_ui = true;
            rect_node.r = r;
            rect_node.g = g;
            rect_node.b = b;
            rect_node.a = a;
            snapshot.push_back(rect_node);
        };

        // 1. Label & Percentage Text (Outside above the control box)
        int pct = static_cast<int>(std::round(get_normalized_value() * 100.0f));
        std::string display_text = m_label + ": " + std::to_string(pct) + "%";

        float font_size = f.get_font_size();
        float label_scale = 0.35f;

        float text_w = 0.0f;
        for (char c : display_text)
        {
            text_w += f.get_glyph(c).xadvance * label_scale;
        }

        float tx = m_x + (m_w - text_w) * 0.5f;
        float ty = m_y - 6.0f;

        uint8_t tr = !m_enabled ? 100 : (selected_or_hovered ? 255 : 200);
        uint8_t tg = !m_enabled ? 100 : (selected_or_hovered ? 255 : 200);
        uint8_t tb = !m_enabled ? 110 : (selected_or_hovered ? 100 : 200);
        uint8_t ta = !m_enabled ? 120 : 255;

        std::vector<render_node> text_nodes = f.get_text_nodes(display_text, tx, ty, label_scale, tr, tg, tb, ta);
        for (auto& node : text_nodes)
        {
            node.is_ui = true;
        }
        snapshot.insert(snapshot.end(), text_nodes.begin(), text_nodes.end());

        // 2. Container Background box
        uint8_t bgr = !m_enabled ? 20 : (selected_or_hovered ? 60 : 30);
        uint8_t bgg = !m_enabled ? 20 : (selected_or_hovered ? 60 : 30);
        uint8_t bgb = !m_enabled ? 25 : (selected_or_hovered ? 80 : 40);
        uint8_t bga = !m_enabled ? 100 : (selected_or_hovered ? 200 : 150);
        add_rect(m_x, m_y, m_w, m_h, bgr, bgg, bgb, bga);

        // 3. Border lines
        uint8_t br = !m_enabled ? 60 : (selected_or_hovered ? 220 : 100);
        uint8_t bg = !m_enabled ? 60 : (selected_or_hovered ? 220 : 100);
        uint8_t bb = !m_enabled ? 70 : (selected_or_hovered ? 255 : 120);
        uint8_t ba = !m_enabled ? 100 : 255;

        add_rect(m_x, m_y, m_w, 2.0f, br, bg, bb, ba);                 // Top border
        add_rect(m_x, m_y + m_h - 2.0f, m_w, 2.0f, br, bg, bb, ba);    // Bottom border
        add_rect(m_x, m_y, 2.0f, m_h, br, bg, bb, ba);                 // Left border
        add_rect(m_x + m_w - 2.0f, m_y, 2.0f, m_h, br, bg, bb, ba);    // Right border

        // 4. Track background bar (Centered vertically inside the slider box)
        float track_padding = 16.0f;
        float track_x = m_x + track_padding;
        float track_w = m_w - (track_padding * 2.0f);
        float track_h = 8.0f;
        float track_y = m_y + (m_h - track_h) * 0.5f;

        add_rect(track_x, track_y, track_w, track_h, 20, 20, 30, !m_enabled ? 100 : 220); // Dark track

        // 5. Filled progress bar
        float norm = get_normalized_value();
        float fill_w = track_w * norm;
        if (fill_w > 0.0f)
        {
            uint8_t fr = !m_enabled ? 70 : (selected_or_hovered ? 255 : 180);
            uint8_t fg = !m_enabled ? 70 : (selected_or_hovered ? 215 : 150);
            uint8_t fb = !m_enabled ? 80 : (selected_or_hovered ? 0 : 50);
            add_rect(track_x, track_y, fill_w, track_h, fr, fg, fb, !m_enabled ? 120 : 255);
        }

        // 6. Knob handle
        float knob_w = 12.0f;
        float knob_h = m_h - 16.0f;
        float knob_x = track_x + fill_w - (knob_w * 0.5f);
        float knob_y = m_y + 8.0f;

        uint8_t kr = !m_enabled ? 100 : (selected_or_hovered ? 255 : 220);
        uint8_t kg = !m_enabled ? 100 : (selected_or_hovered ? 255 : 220);
        uint8_t kb = !m_enabled ? 110 : (selected_or_hovered ? 255 : 220);
        add_rect(knob_x, knob_y, knob_w, knob_h, kr, kg, kb, !m_enabled ? 140 : 255);
    }
}
