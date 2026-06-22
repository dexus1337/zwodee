#pragma once

/**
 * @file    animated-object.hpp
 * @author  dexus1337
 * @brief   Defines the animated_object class representing non-static textured objects with animations.
 * @version 1.0
 * @date    21.06.2026
 */

#include "entities/textured-object.hpp"
#include "entities/animator.hpp"

namespace zwodee
{
    /**
     * @class   animated_object
     * @brief   A textured game object that supports sprite sheet animations (e.g. torches, coins, portals).
     */
    class ZWODEE_API animated_object : public textured_object
    {
    public:
        animated_object(uint32_t network_id, const texture* tex, int total_frames, int ticks_per_frame, bool loops = true);

        /**
         * @brief Updates the animation state.
         */
        void tick() override;

        /**
         * @brief Renders the current frame of the animation.
         */
        void render(renderer& target_renderer, double alpha) override;

        /**
         * @brief Extracts a lightweight render snapshot description node (lock-free).
         */
        render_node get_render_node() const override;

        /**
         * @brief Configures animation properties.
         */
        void configure_animator(int total_frames, int ticks_per_frame, bool loops);

    protected:
        animator m_animator;
        int m_sprite_cols = 1; // Grid columns of the animation sprite sheet
    };
}
