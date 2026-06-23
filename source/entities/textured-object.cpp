#include "entities/textured-object.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"

namespace zwodee
{
    textured_object::textured_object(uint32_t network_id, const texture* tex)
        : game_object(network_id), m_texture(tex)
    {
    }

    void textured_object::render(renderer& target_renderer, double alpha)
    {
        // Guard clause: Avoid rendering with null texture
        if (!m_texture)
        {
            return;
        }

        // Standard render using interpolated coordinates
        float render_x = m_x + (m_vx * static_cast<float>(alpha));
        float render_y = m_y + (m_vy * static_cast<float>(alpha));

        target_renderer.draw_sprite(*m_texture, 0, 0, m_texture->get_width(), m_texture->get_height(), render_x, render_y, m_width, m_height, m_flip_horizontal);
    }

    render_node textured_object::get_render_node() const
    {
        if (!m_texture)
        {
            return render_node{ m_x, m_y, m_width, m_height, nullptr, 0, 0, 0, 0, false };
        }
        return render_node{ m_x, m_y, m_width, m_height, m_texture, 0, 0, m_texture->get_width(), m_texture->get_height(), m_flip_horizontal };
    }

    void textured_object::set_texture(const texture* tex)
    {
        m_texture = tex;
    }

    const texture* textured_object::get_texture() const
    {
        return m_texture;
    }

    void textured_object::set_flip_horizontal(bool flip)
    {
        m_flip_horizontal = flip;
    }

    bool textured_object::get_flip_horizontal() const
    {
        return m_flip_horizontal;
    }
}
