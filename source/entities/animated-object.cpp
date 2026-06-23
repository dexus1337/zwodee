#include "entities/animated-object.hpp"
#include "graphics/texture.hpp"
#include "graphics/renderer.hpp"

namespace zwodee
{
    animated_object::animated_object(uint32_t network_id, const texture* tex, int total_frames, int ticks_per_frame, bool loops)
        : textured_object(network_id, tex), m_animator(total_frames, ticks_per_frame, loops), m_sprite_cols(total_frames)
    {
    }

    void animated_object::tick()
    {
        // Advance animation by 1 tick
        m_animator.update(1);
    }

    void animated_object::render(renderer& target_renderer, double alpha)
    {
        // Guard clause: Avoid rendering with null texture
        if (!m_texture)
        {
            return;
        }

        // Interpolated position
        float render_x = m_x + (m_vx * static_cast<float>(alpha));
        float render_y = m_y + (m_vy * static_cast<float>(alpha));

        // Source rectangle calculation based on animator's active frame
        int frame = m_animator.get_current_frame();
        int frame_width = m_texture->get_width() / m_sprite_cols;
        int frame_height = m_texture->get_height();

        int src_x = (frame % m_sprite_cols) * frame_width;
        int src_y = 0; // Single row spritesheets

        target_renderer.draw_sprite(*m_texture, src_x, src_y, frame_width, frame_height, render_x, render_y, m_width, m_height, m_flip_horizontal);
    }

    render_node animated_object::get_render_node() const
    {
        if (!m_texture)
        {
            return render_node{ m_x, m_y, m_width, m_height, nullptr, 0, 0, 0, 0, false };
        }
        int frame = m_animator.get_current_frame();
        int frame_width = m_texture->get_width() / m_sprite_cols;
        int frame_height = m_texture->get_height();

        int src_x = (frame % m_sprite_cols) * frame_width;
        int src_y = 0;

        return render_node{ m_x, m_y, m_width, m_height, m_texture, src_x, src_y, frame_width, frame_height, m_flip_horizontal };
    }

    void animated_object::configure_animator(int total_frames, int ticks_per_frame, bool loops)
    {
        m_sprite_cols = total_frames;
        m_animator.set_animation(total_frames, ticks_per_frame, loops);
    }
}
