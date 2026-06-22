#pragma once

/**
 * @file    entity.hpp
 * @author  dexus1337
 * @brief   Defines the entity class, representing dynamic characters with health and animations.
 * @version 1.0
 * @date    21.06.2026
 */

#include "entities/animated-object.hpp"

namespace zwodee
{
    /**
     * @class   entity
     * @brief   Base class for all active entities (players, enemies, NPCs) with lifecycle properties.
     */
    class ZWODEE_API entity : public animated_object
    {
    public:
        entity(uint32_t network_id, const texture* tex, int max_health);

        /**
         * @brief Updates physics and animator.
         */
        void tick() override;

        /**
         * @brief Applies damage to the entity.
         */
        virtual void take_damage(int amount);

        // Getters and setters
        [[nodiscard]] int get_health() const;
        [[nodiscard]] int get_max_health() const;
        [[nodiscard]] bool is_dead() const;

        void set_health(int health);

    protected:
        int m_health = 100;
        int m_max_health = 100;
    };
}
