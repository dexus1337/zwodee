#pragma once

/**
 * @file    static-object.hpp
 * @author  dexus1337
 * @brief   Defines the static_object class representing stationary environment obstacles.
 * @version 1.0
 * @date    21.06.2026
 */

#include "entities/textured-object.hpp"

namespace zwodee
{
    /**
     * @class   static_object
     * @brief   A static obstacle or tile element in the game level (non-moving).
     */
    class ZWODEE_API static_object : public textured_object
    {
    public:
        static_object(uint32_t network_id, const texture* tex);

        /**
         * @brief Override tick. Static objects do not move or update positions.
         */
        void tick() override;
    };
}
