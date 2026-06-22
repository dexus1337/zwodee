#include "entities/entity.hpp"

namespace zwodee
{
    entity::entity(uint32_t network_id, const texture* tex, int max_health)
        : animated_object(network_id, tex, 1, 1, true), m_health(max_health), m_max_health(max_health)
    {
    }

    void entity::tick()
    {
        // Update physics movement
        game_object::tick();

        // Update animation frames by 1 tick
        animated_object::tick();
    }

    void entity::take_damage(int amount)
    {
        m_health -= amount;

        // Guard clause: Cap health at 0
        if (m_health < 0)
        {
            m_health = 0;
        }
    }

    int entity::get_health() const
    {
        return m_health;
    }

    int entity::get_max_health() const
    {
        return m_max_health;
    }

    bool entity::is_dead() const
    {
        return m_health <= 0;
    }

    void entity::set_health(int health)
    {
        m_health = health;
        
        // Guard clause: Cap health
        if (m_health > m_max_health)
        {
            m_health = m_max_health;
        }
    }
}
