#pragma once

/**
 * @file    entity-player.hpp
 * @author  dexus1337
 * @brief   Defines the entity_player class, controlling character positions using player inputs.
 * @version 1.0
 * @date    21.06.2026
 */

#include "entities/entity.hpp"
#include "core/input.hpp"

namespace zwodee
{
    /**
     * @class   entity_player
     * @brief   An entity controlled directly by local or network inputs.
     */
    class ZWODEE_API entity_player : public entity
    {
    public:
        entity_player(uint32_t network_id, const texture* tex, int max_health);

        void tick() override;

        inline float get_speed() const { return m_speed; }

        inline void set_input(const input_state& input) { m_current_input = input; }
        inline void set_speed(float val) { m_speed = val; }

    protected:
        input_state m_current_input;

    private:
        float m_speed = 1.0f;
    };
}
