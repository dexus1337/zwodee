#pragma once

/**
 * @file    level-format.hpp
 * @author  dexus1337
 * @brief   Defines the binary structures and layout for the custom Zwodee Level (.zwl) format.
 * @version 1.0
 * @date    21.06.2026
 */

#include <cstdint>

namespace zwodee
{
    #pragma pack(push, 1)

    /**
     * @struct  level_header
     * @brief   Binary header at the beginning of every .zwl file.
     */
    struct level_header
    {
        char magic[4];          // "ZWL\0"
        uint32_t version;       // Format version (e.g., 1)
        uint32_t width;         // Map width in tiles
        uint32_t height;        // Map height in tiles
        uint32_t tile_count;    // Total tiles (width * height)
        uint32_t entity_count;  // Total entities stored in the level
    };

    /**
     * @struct  binary_tile
     * @brief   Represents a single static tile's collision/graphical index.
     */
    struct binary_tile
    {
        uint16_t tile_id;       // Index in the tileset (0 = empty)
        uint8_t flags;          // Bit 0: solid, Bit 1: hazard, etc.
    };

    /**
     * @struct  binary_entity
     * @brief   Represents dynamic entities spawned in the level.
     */
    struct binary_entity
    {
        uint32_t type_id;       // 1 = Player, 2 = AI (Enemy), etc.
        float x;                // Spawn coordinate X
        float y;                // Spawn coordinate Y
        int32_t health;         // Starting health
    };

    #pragma pack(pop)
}
