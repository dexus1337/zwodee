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

    std::vector<uint16_t> tiles;
    std::vector<zwodee::binary_entity> bin_entities;

    if (root.contains("layers") && root["layers"].is_array())
    {
        // Tiled format
        for (const auto& layer : root["layers"])
        {
            std::string layer_type = layer.value("type", "");
            if (layer_type == "tilelayer" && tiles.empty())
            {
                tiles = layer.value("data", std::vector<uint16_t>());
            }
            else if (layer_type == "objectgroup" && layer.contains("objects") && layer["objects"].is_array())
            {
                for (const auto& obj : layer["objects"])
                {
                    zwodee::binary_entity ent;
                    std::string type = obj.value("type", "");
                    uint32_t gid = obj.value("gid", 0);

                    // Support Tile Objects by inferring type from local tile ID
                    if (type.empty() && gid > 0)
                    {
                        uint32_t clean_gid = gid & 0x1FFFFFFF; // Clear Tiled flip flags
                        
                        uint32_t tile_id = 0;
                        if (root.contains("tilesets"))
                        {
                            uint32_t best_firstgid = 1;
                            for (const auto& ts : root["tilesets"])
                            {
                                uint32_t fg = ts.value("firstgid", 1);
                                if (fg <= clean_gid && fg > best_firstgid)
                                {
                                    best_firstgid = fg;
                                }
                            }
                            tile_id = clean_gid - best_firstgid;
                        }
                        else
                        {
                            tile_id = clean_gid - 1;
                        }

                        switch (tile_id)
                        {
                            case 2: type = "player"; break;
                            case 3: type = "mummy"; break;
                            case 4: type = "soldier"; break;
                            case 5: type = "vampire"; break;
                            case 6: type = "dragon"; break;
                            case 7: type = "stone"; break;
                            case 8: type = "diamond"; break;
                            case 9: type = "gold_coin"; break;
                            case 10: type = "lamp"; break;
                            case 11: type = "garlic"; break;
                            case 12: type = "onion"; break;
                            case 13: type = "pickaxe"; break;
                            case 14: type = "exit_door"; break;
                        }
                    }

                    if (type == "player") ent.type_id = 1;
                    else if (type == "mummy") ent.type_id = 10;
                    else if (type == "soldier") ent.type_id = 11;
                    else if (type == "vampire") ent.type_id = 12;
                    else if (type == "dragon") ent.type_id = 13;
                    else if (type == "stone") ent.type_id = 20;
                    else if (type == "diamond") ent.type_id = 21;
                    else if (type == "gold_coin") ent.type_id = 22;
                    else if (type == "lamp") ent.type_id = 23;
                    else if (type == "garlic") ent.type_id = 24;
                    else if (type == "onion") ent.type_id = 25;
                    else if (type == "pickaxe") ent.type_id = 26;
                    else if (type == "exit_door") ent.type_id = 27;
                    else if (type == "enemy") ent.type_id = 2; // default AI
                    else
                    {
                        std::cerr << "Warning: Unknown entity type ignored: " << type << "\n";
                        continue;
                    }

                    ent.x = obj.value("x", 0.0f);
                    ent.y = obj.value("y", 0.0f);

                    // Tile objects in Tiled are anchored at the bottom-left. Adjust Y up by height.
                    // Tile objects in Tiled are anchored at the bottom-left. Adjust Y up by height.
                    if (gid > 0)
                    {
                        ent.y -= obj.value("height", 32.0f);
                    }
                    ent.health = 100;

                    if (obj.contains("properties") && obj["properties"].is_array())
                    {
                        for (const auto& prop : obj["properties"])
                        {
                            if (prop.value("name", "") == "health")
                            {
                                ent.health = prop.value("value", 100);
                                break;
                            }
                        }
                    }
                    bin_entities.push_back(ent);
                }
            }
        }
    }
    else
    {
        // Old custom format
        tiles = root.value("tiles", std::vector<uint16_t>());
        
        if (root.contains("entities") && root["entities"].is_array())
        {
            for (const auto& item : root["entities"])
            {
                zwodee::binary_entity ent;
                std::string type = item.value("type", "");

                if (type == "player") ent.type_id = 1;
                else if (type == "enemy") ent.type_id = 2;
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
    header.target_score = -1;

    if (root.contains("properties") && root["properties"].is_array())
    {
        for (const auto& prop : root["properties"])
        {
            if (prop.value("name", "") == "required_coins")
            {
                header.target_score = prop.value("value", -1);
            }
        }
    }

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
