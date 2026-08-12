#include "ui/button.hpp"

namespace zwodee
{
    button::button(const std::string& text, float x, float y, float w, float h)
        : m_text(text), m_x(x), m_y(y), m_w(w), m_h(h)
    {
    }

    bool button::is_hovered(float mx, float my) const
    {
        if (!m_enabled) return false;
        return mx >= m_x && mx <= m_x + m_w && my >= m_y && my <= m_y + m_h;
    }

    void button::add_to_snapshot(render_snapshot& snapshot, const font& f, bool selected_or_hovered) const
    {
        // 1. Background node (solid color rect)
        render_node bg_node{};
        bg_node.x = m_x;
        bg_node.y = m_y;
        bg_node.w = m_w;
        bg_node.h = m_h;
        bg_node.tex = nullptr;
        bg_node.is_ui = true;
        if (!m_enabled)
        {
            bg_node.r = 20; bg_node.g = 20; bg_node.b = 25; bg_node.a = 100;
        }
        else if (selected_or_hovered)
        {
            bg_node.r = 60; bg_node.g = 60; bg_node.b = 80; bg_node.a = 200;
        }
        else
        {
            bg_node.r = 30; bg_node.g = 30; bg_node.b = 40; bg_node.a = 150;
        }
        snapshot.push_back(bg_node);

        // 2. Border lines (4 thin rects)
        uint8_t br = !m_enabled ? 60 : (selected_or_hovered ? 220 : 100);
        uint8_t bg = !m_enabled ? 60 : (selected_or_hovered ? 220 : 100);
        uint8_t bb = !m_enabled ? 70 : (selected_or_hovered ? 255 : 120);
        uint8_t ba = !m_enabled ? 100 : 255;

        auto add_border_rect = [&](float bx, float by, float bw, float bh) {
            render_node border_node{};
            border_node.x = bx;
            border_node.y = by;
            border_node.w = bw;
            border_node.h = bh;
            border_node.tex = nullptr;
            border_node.is_ui = true;
            border_node.r = br;
            border_node.g = bg;
            border_node.b = bb;
            border_node.a = ba;
            snapshot.push_back(border_node);
        };

        add_border_rect(m_x, m_y, m_w, 2.0f);                 // Top
        add_border_rect(m_x, m_y + m_h - 2.0f, m_w, 2.0f);    // Bottom
        add_border_rect(m_x, m_y, 2.0f, m_h);                 // Left
        add_border_rect(m_x + m_w - 2.0f, m_y, 2.0f, m_h);    // Right

        // 3. Text nodes
        float font_size = f.get_font_size();
        float scale = (m_h - 12.0f) / font_size;
        if (scale > 1.0f) scale = 1.0f;

        float text_w = 0.0f;
        for (char c : m_text)
        {
            text_w += f.get_glyph(c).xadvance * scale;
        }

        float tx = m_x + (m_w - text_w) * 0.5f;
        float ty = m_y + m_h * 0.5f + (font_size * scale) * 0.3f;

        uint8_t tr = !m_enabled ? 100 : (selected_or_hovered ? 255 : 200);
        uint8_t tg = !m_enabled ? 100 : (selected_or_hovered ? 255 : 200);
        uint8_t tb = !m_enabled ? 110 : (selected_or_hovered ? 100 : 200);
        uint8_t ta = !m_enabled ? 120 : 255;

        std::vector<render_node> text_nodes = f.get_text_nodes(m_text, tx, ty, scale, tr, tg, tb, ta);
        for (auto& node : text_nodes)
        {
            node.is_ui = true;
        }
        snapshot.insert(snapshot.end(), text_nodes.begin(), text_nodes.end());
    }
}
