#include "entities/entity-player.hpp"

namespace zwodee
{
    entity_player::entity_player(uint32_t network_id, const texture* tex, int max_health)
        : entity(network_id, tex, max_health)
    {
    }

    void entity_player::set_input(const input_state& input)
    {
        m_current_input = input;
    }

    void entity_player::tick()
    {
        m_vx = 0.0f;
        m_vy = 0.0f;

        // Apply inputs to horizontal velocity
        if (m_current_input.is_down(input_state::move_left))
        {
            m_vx = -m_speed;
        }
        else if (m_current_input.is_down(input_state::move_right))
        {
            m_vx = m_speed;
        }

        // Apply inputs to vertical velocity
        if (m_current_input.is_down(input_state::move_up))
        {
            m_vy = -m_speed;
        }
        else if (m_current_input.is_down(input_state::move_down))
        {
            m_vy = m_speed;
        }

        m_x += m_vx;
        m_y += m_vy;

        // Advance animation
        m_animator.update(1);
    }

    float entity_player::get_speed() const
    {
        return m_speed;
    }

    void entity_player::set_speed(float val)
    {
        m_speed = val;
    }
}
