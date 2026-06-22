#include "levels/level-format.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using json = nlohmann::json;

int main(int argc, char** argv)
{
    // Guard clause: Check command line arguments
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_map.json> <output_map.zwl>\n";
        return 1;
    }

    std::string input_path = argv[1];
    std::string output_path = argv[2];

    std::ifstream input_file(input_path);
    
    // Guard clause: Verify input file opens
    if (!input_file.is_open())
    {
        std::cerr << "Error: Could not open input file: " << input_path << "\n";
        return 1;
    }

    json root;
    try
    {
        input_file >> root;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
        return 1;
    }

    // Read map dimensions
    uint32_t width = root.value("width", 0);
    uint32_t height = root.value("height", 0);

    // Guard clause: Verify dimensions
    if (width == 0 || height == 0)
    {
        std::cerr << "Error: Invalid map dimensions (width and height must be > 0)\n";
        return 1;
    }

    // Read tile data
    std::vector<uint16_t> tiles = root.value("tiles", std::vector<uint16_t>());
    
    // Guard clause: Verify tile count matches dimensions
    if (tiles.size() != width * height)
    {
        std::cerr << "Error: Tile data array size (" << tiles.size() 
                  << ") does not match map dimensions (" << width * height << ")\n";
        return 1;
    }

    // Process entities
    std::vector<zwodee::binary_entity> bin_entities;
    if (root.contains("entities") && root["entities"].is_array())
    {
        for (const auto& item : root["entities"])
        {
            zwodee::binary_entity ent;
            std::string type = item.value("type", "");

            if (type == "player")
            {
                ent.type_id = 1;
            }
            else if (type == "enemy")
            {
                ent.type_id = 2;
            }
            else
            {
                std::cerr << "Warning: Unknown entity type ignored: " << type << "\n";
                continue;
            }

            ent.x = item.value("x", 0.0f);
            ent.y = item.value("y", 0.0f);
            ent.health = item.value("health", 100);
            bin_entities.push_back(ent);
        }
    }

    // Open output file
    std::ofstream output_file(output_path, std::ios::binary);

    // Guard clause: Verify output file opens
    if (!output_file.is_open())
    {
        std::cerr << "Error: Could not open output file: " << output_path << "\n";
        return 1;
    }

    // Create binary header
    zwodee::level_header header;
    std::memcpy(header.magic, "ZWL\0", 4);
    header.version = 1;
    header.width = width;
    header.height = height;
    header.tile_count = width * height;
    header.entity_count = static_cast<uint32_t>(bin_entities.size());

    // Write header
    output_file.write(reinterpret_cast<const char*>(&header), sizeof(zwodee::level_header));

    // Convert and write tiles
    std::vector<zwodee::binary_tile> bin_tiles(header.tile_count);
    for (size_t i = 0; i < tiles.size(); ++i)
    {
        bin_tiles[i].tile_id = tiles[i];
        bin_tiles[i].flags = (tiles[i] != 0) ? 1 : 0; // Solid if tile_id is non-zero
    }
    output_file.write(reinterpret_cast<const char*>(bin_tiles.data()), bin_tiles.size() * sizeof(zwodee::binary_tile));

    // Write entities
    if (!bin_entities.empty())
    {
        output_file.write(reinterpret_cast<const char*>(bin_entities.data()), bin_entities.size() * sizeof(zwodee::binary_entity));
    }

    std::cout << "Level compiled successfully to " << output_path << " (" 
              << width << "x" << height << ", " << bin_entities.size() << " entities)\n";

    return 0;
}
