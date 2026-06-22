#pragma once

/**
 * @file    level-loader.hpp
 * @author  dexus1337
 * @brief   Defines the level_loader class for reading and constructing levels from .zwl binary files.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"
#include <memory>
#include <string>

namespace zwodee
{
    class tile_level;
    class texture;

    /**
     * @class   level_loader
     * @brief   Static utility class providing level parsing capabilities.
     */
    class ZWODEE_API level_loader
    {
    public:
        /**
         * @brief Loads a level from a custom binary .zwl file.
         * @param file_path Path to the compiled binary level file.
         * @param tileset_tex Texture pointer used for static grid tiles.
         * @param player_tex Texture pointer used for player entity spawn.
         * @param enemy_tex Texture pointer used for AI NPC spawns.
         * @return A constructed tile_level, or nullptr on failure.
         */
        static std::unique_ptr<tile_level> load_from_binary(
            const std::string& file_path,
            const texture* tileset_tex,
            const texture* player_tex,
            const texture* enemy_tex
        );
    };
}
