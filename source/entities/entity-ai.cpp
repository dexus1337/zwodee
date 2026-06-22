#include "entities/entity-ai.hpp"
#include <cmath>

namespace zwodee
{
    entity_ai::entity_ai(uint32_t network_id, const texture* tex, int max_health)
        : entity(network_id, tex, max_health)
    {
    }

    void entity_ai::tick()
    {
        
        m_vx = 0.0f;
        m_vy = 0.0f;

        // Run behaviors based on state
        if (m_state == state_patrol)
        {
            // Patrol horizontal movement
            if (m_patrol_moving_right)
            {
                m_vx = m_speed;
                m_x += m_vx;
                
                // Guard clause to turn around
                if (m_x >= m_patrol_end_x)
                {
                    m_x = m_patrol_end_x;
                    m_patrol_moving_right = false;
                }
            }
            else
            {
                m_vx = -m_speed;
                m_x += m_vx;

                // Guard clause to turn around
                if (m_x <= m_patrol_start_x)
                {
                    m_x = m_patrol_start_x;
                    m_patrol_moving_right = true;
                }
            }
        }
        else if (m_state == state_chase)
        {
            // Simple vector chasing
            float dx = m_target_x - m_x;
            float dy = m_target_y - m_y;
            float dist = std::sqrt(dx * dx + dy * dy);

            // Guard clause: Avoid divide by zero if at target
            if (dist > 1.0f)
            {
                m_vx = (dx / dist) * m_speed;
                m_vy = (dy / dist) * m_speed;
                m_x += m_vx;
                m_y += m_vy;
            }
        }

        m_animator.update(1);
    }

    void entity_ai::set_target(float tx, float ty)
    {
        m_target_x = tx;
        m_target_y = ty;
    }

    entity_ai::ai_state entity_ai::get_state() const
    {
        return m_state;
    }

    void entity_ai::set_state(ai_state st)
    {
        m_state = st;
    }
}
