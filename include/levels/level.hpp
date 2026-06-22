#pragma once

/**
 * @file    level.hpp
 * @author  dexus1337
 * @brief   Defines the level interface class for level-based gameplay.
 * @version 1.0
 * @date    21.06.2026
 */

#include "graphics/render-snapshot.hpp"

namespace zwodee
{
    class renderer;
    struct input_state;

    /**
     * @class   level
     * @brief   Abstract base class for all game levels (OOP structure).
     */
    class ZWODEE_API level
    {
    public:
        level() = default;
        virtual ~level() = default;

        // Prevent copying
        level(const level&) = delete;
        level& operator=(const level&) = delete;

        /**
         * @brief Called when transitioning into this level.
         */
        virtual void on_enter() = 0;

        /**
         * @brief Called when transitioning out of this level.
         */
        virtual void on_exit() = 0;

        /**
         * @brief Set local player inputs for this tick.
         */
        virtual void set_player_input(const input_state& input) { (void)input; }

        /**
         * @brief Updates the level logic (128Hz ticks).
         */
        virtual void tick() = 0;

        /**
         * @brief Renders the level elements.
         */
        virtual void render(renderer& target_renderer, double alpha) = 0;

        /**
         * @brief Captures a lightweight copy of all visual coordinates lock-free.
         */
        virtual render_snapshot get_render_snapshot(int display_w, int display_h) const = 0;
    };
}
