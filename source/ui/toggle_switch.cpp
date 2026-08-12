#include "ui/toggle_switch.hpp"
#include <cmath>

namespace zwodee
{
    toggle_switch::toggle_switch(const std::string& label, bool is_on, float x, float y, float w, float h)
        : m_label(label), m_on(is_on), m_x(x), m_y(y), m_w(w), m_h(h)
    {
    }

    bool toggle_switch::is_hovered(float mx, float my) const
    {
        if (!m_enabled) return false;
        return mx >= m_x && mx <= m_x + m_w && my >= m_y && my <= m_y + m_h;
    }

    void toggle_switch::add_to_snapshot(render_snapshot& snapshot, const font& f, bool selected_or_hovered) const
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

        // 1. Render Label Text (Outside above the control box)
        float font_size = f.get_font_size();
        float label_scale = 0.35f;

        float label_w = 0.0f;
        for (char c : m_label)
        {
            label_w += f.get_glyph(c).xadvance * label_scale;
        }

        float lx = m_x + (m_w - label_w) * 0.5f;
        float ly = m_y - 6.0f;

        uint8_t lr = m_enabled ? (selected_or_hovered ? 255 : 200) : 100;
        uint8_t lg = m_enabled ? (selected_or_hovered ? 255 : 200) : 100;
        uint8_t lb = m_enabled ? (selected_or_hovered ? 100 : 200) : 110;
        uint8_t la = m_enabled ? 255 : 120;

        std::vector<render_node> label_nodes = f.get_text_nodes(m_label, lx, ly, label_scale, lr, lg, lb, la);
        for (auto& node : label_nodes) node.is_ui = true;
        snapshot.insert(snapshot.end(), label_nodes.begin(), label_nodes.end());

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

        add_rect(m_x, m_y, m_w, 2.0f, br, bg, bb, ba);                 // Top
        add_rect(m_x, m_y + m_h - 2.0f, m_w, 2.0f, br, bg, bb, ba);    // Bottom
        add_rect(m_x, m_y, 2.0f, m_h, br, bg, bb, ba);                 // Left
        add_rect(m_x + m_w - 2.0f, m_y, 2.0f, m_h, br, bg, bb, ba);    // Right

        // 4. Switch Pill Track
        float pill_w = 70.0f;
        float pill_h = m_h - 16.0f;
        float pill_x = m_x + (m_w - pill_w) * 0.5f;
        float pill_y = m_y + 8.0f;

        uint8_t tr = !m_enabled ? 40 : (m_on ? 40 : 50);
        uint8_t tg = !m_enabled ? 40 : (m_on ? 180 : 50);
        uint8_t tb = !m_enabled ? 45 : (m_on ? 120 : 60);
        uint8_t ta = !m_enabled ? 120 : 230;
        add_rect(pill_x, pill_y, pill_w, pill_h, tr, tg, tb, ta);

        // 5. Switch Knob
        float knob_w = pill_h - 4.0f;
        float knob_h = pill_h - 4.0f;
        float knob_x = m_on ? (pill_x + pill_w - knob_w - 2.0f) : (pill_x + 2.0f);
        float knob_y = pill_y + 2.0f;

        uint8_t kr = !m_enabled ? 100 : 255;
        uint8_t kg = !m_enabled ? 100 : 255;
        uint8_t kb = !m_enabled ? 105 : 255;
        add_rect(knob_x, knob_y, knob_w, knob_h, kr, kg, kb, 255);

        // 6. "ON" / "OFF" text inside pill
        std::string status_text = m_on ? "ON" : "OFF";
        float status_scale = 0.28f;
        float status_w = 0.0f;
        for (char c : status_text)
        {
            status_w += f.get_glyph(c).xadvance * status_scale;
        }

        float sx = m_on ? (pill_x + 8.0f) : (pill_x + pill_w - status_w - 8.0f);
        float sy = pill_y + pill_h * 0.72f;

        std::vector<render_node> status_nodes = f.get_text_nodes(status_text, sx, sy, status_scale, 255, 255, 255, m_enabled ? 255 : 120);
        for (auto& node : status_nodes) node.is_ui = true;
        snapshot.insert(snapshot.end(), status_nodes.begin(), status_nodes.end());
    }
}
