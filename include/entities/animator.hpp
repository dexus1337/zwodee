#pragma once

/**
 * @file    animator.hpp
 * @author  dexus1337
 * @brief   Defines the animator class for managing sprite-sheet based frame animations.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"

namespace zwodee
{
    /**
     * @class   animator
     * @brief   Handles calculations for current frames of animation based on ticks.
     */
    class ZWODEE_API animator
    {
    public:
        /**
         * @brief Constructs an animator.
         * @param total_frames Number of frames in the animation sequence.
         * @param ticks_per_frame Number of 128Hz ticks to display each frame.
         * @param loops Whether the animation loops continuously.
         */
        animator(int total_frames, int ticks_per_frame, bool loops = true);

        /**
         * @brief Updates the animation frame counter by a number of ticks.
         */
        void update(int ticks);

        /**
         * @brief Reset animation to the beginning.
         */
        void reset();

        /**
         * @brief Gets the current frame index.
         */
        [[nodiscard]] int get_current_frame() const;

        /**
         * @brief Set animation attributes.
         */
        void set_animation(int total_frames, int ticks_per_frame, bool loops = true);

    private:
        int m_total_frames = 1;
        int m_ticks_per_frame = 1;
        bool m_loops = true;

        int m_current_frame = 0;
        int m_accumulated_ticks = 0;
    };
}
