#pragma once

/**
 * @file    input.hpp
 * @author  dexus1337
 * @brief   Defines input structures and input state mapping, serialized for multiplayer support.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"
#include <cstdint>
#include <vector>

namespace zwodee
{
    /**
     * @struct  input_state
     * @brief   Represents the physical buttons/directions pressed in a single frame/tick.
     *          Designed to be small and serializeable for multiplayer synchronization.
     */
    struct ZWODEE_API input_state
    {
        uint32_t buttons = 0; // Bitmask of buttons pressed (e.g. up, down, left, right, action1, action2)

        enum button_mask : uint32_t
        {
            move_up    = 1 << 0,
            move_down  = 1 << 1,
            move_left  = 1 << 2,
            move_right = 1 << 3,
            action_1   = 1 << 4, // e.g. Jump / Shoot
            action_2   = 1 << 5, // e.g. Pause / Menu
            quit       = 1 << 6
        };

        /**
         * @brief Check if a specific button is down.
         */
        [[nodiscard]] bool is_down(button_mask button) const;

        /**
         * @brief Serializes the input state to a byte buffer.
         */
        void serialize(std::vector<uint8_t>& buffer) const;

        /**
         * @brief Deserializes the input state from a byte buffer.
         */
        void deserialize(const std::vector<uint8_t>& buffer, size_t& offset);
    };
}
