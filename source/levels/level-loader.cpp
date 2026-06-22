#include "levels/level-loader.hpp"
#include "levels/level-format.hpp"
#include "levels/tile-level.hpp"
#include "entities/entity-player.hpp"
#include "entities/entity-ai.hpp"
#include <fstream>
#include <vector>
#include <cstring>

namespace zwodee
{
    std::unique_ptr<tile_level> level_loader::load_from_binary(
        const std::string& file_path,
        const texture* tileset_tex,
        const texture* player_tex,
        const texture* enemy_tex)
    {
        std::ifstream file(file_path, std::ios::binary);

        // Guard clause: Check if file opened successfully
        if (!file.is_open())
        {
            return nullptr;
        }

        level_header header;
        file.read(reinterpret_cast<char*>(&header), sizeof(level_header));

        // Guard clause: Verify magic header identifier
        if (std::memcmp(header.magic, "ZWL\0", 4) != 0)
        {
            return nullptr;
        }

        // Guard clause: Verify format version
        if (header.version != 1)
        {
            return nullptr;
        }

        auto lvl = std::make_unique<tile_level>(header.width, header.height);

        // Read static grid tiles
        std::vector<binary_tile> binary_tiles(header.tile_count);
        file.read(reinterpret_cast<char*>(binary_tiles.data()), header.tile_count * sizeof(binary_tile));

        // Populate tile objects in level
        for (uint32_t y = 0; y < header.height; ++y)
        {
            for (uint32_t x = 0; x < header.width; ++x)
            {
                const auto& bin_tile = binary_tiles[y * header.width + x];
                
                // Guard clause: Skip empty tiles
                if (bin_tile.tile_id == 0)
                {
                    continue;
                }

                lvl->set_tile(x, y, bin_tile.tile_id, bin_tile.flags, tileset_tex);
            }
        }

        // Read entities array
        std::vector<binary_entity> binary_entities(header.entity_count);
        if (header.entity_count > 0)
        {
            file.read(reinterpret_cast<char*>(binary_entities.data()), header.entity_count * sizeof(binary_entity));
        }

        // Spawn dynamic entities
        for (const auto& bin_ent : binary_entities)
        {
            std::unique_ptr<entity> ent;

            if (bin_ent.type_id == 1) // 1 = Player
            {
                ent = std::make_unique<entity_player>(0, player_tex, bin_ent.health);
            }
            else if (bin_ent.type_id == 2) // 2 = AI Enemy
            {
                ent = std::make_unique<entity_ai>(0, enemy_tex, bin_ent.health);
            }

            // Guard clause: Skip unsupported entity type
            if (!ent)
            {
                continue;
            }

            ent->set_position(bin_ent.x, bin_ent.y);
            lvl->add_entity(std::move(ent));
        }

        return lvl;
    }
}
