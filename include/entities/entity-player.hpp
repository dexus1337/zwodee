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

        /**
         * @brief Sets the input state for the next update tick.
         */
        void set_input(const input_state& input);

        /**
         * @brief Override tick to process button moves.
         */
        void tick() override;

        /**
         * @brief Gets current speed.
         */
        [[nodiscard]] float get_speed() const;

        /**
         * @brief Sets movement speed.
         */
        void set_speed(float val);

    protected:
        input_state m_current_input;

    private:
        float m_speed = 1.0f;
    };
}
