#pragma once

/**
 * @file    entity-ai.hpp
 * @author  dexus1337
 * @brief   Defines the entity_ai class representing autonomous non-player characters.
 * @version 1.0
 * @date    21.06.2026
 */

#include "entities/entity.hpp"

namespace zwodee
{
    /**
     * @class   entity_ai
     * @brief   An autonomous entity running basic state machine behaviors.
     */
    class ZWODEE_API entity_ai : public entity
    {
    public:
        enum ai_state : int
        {
            state_idle,
            state_patrol,
            state_chase
        };

        entity_ai(uint32_t network_id, const texture* tex, int max_health);

        /**
         * @brief Override tick. Instead of checking player inputs, runs autonomous AI behaviors.
         */
        void tick() override;

        /**
         * @brief Sets target coordinate to chase.
         */
        void set_target(float tx, float ty);

        // Getters and setters
        [[nodiscard]] ai_state get_state() const;
        void set_state(ai_state st);

    private:
        ai_state m_state = state_patrol;
        float m_speed = 1.5f;

        // Patrol limits
        float m_patrol_start_x = 0.0f;
        float m_patrol_end_x = 200.0f;
        bool m_patrol_moving_right = true;

        // Target coordinates
        float m_target_x = 0.0f;
        float m_target_y = 0.0f;
    };
}
