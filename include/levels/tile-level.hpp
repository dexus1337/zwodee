#pragma once

/**
 * @file    tile-level.hpp
 * @author  dexus1337
 * @brief   Defines the tile_level class representing a playable level made of static tiles and dynamic entities.
 * @version 1.0
 * @date    21.06.2026
 */

#include "levels/level.hpp"
#include "levels/level-format.hpp"
#include <vector>
#include <memory>

namespace zwodee
{
    class static_object;
    class entity;
    class texture;

    /**
     * @class   tile_level
     * @brief   An implementation of level that contains tile grids and dynamic entities.
     */
    class ZWODEE_API tile_level : public level
    {
    public:
        tile_level(uint32_t width, uint32_t height);
        ~tile_level() override;

        void on_enter() override;
        void on_exit() override;
        void set_player_input(const input_state& input) override;
        void tick() override;
        void render(renderer& target_renderer, double alpha) override;

        render_snapshot get_render_snapshot(int display_w, int display_h) const override;

        // Populate methods
        void set_tile(uint32_t x, uint32_t y, uint16_t tile_id, uint8_t flags, const texture* tex);
        void add_entity(std::unique_ptr<entity> ent);
        void set_background_texture(const texture* tex);

        // Getters
        [[nodiscard]] uint32_t get_width() const;
        [[nodiscard]] uint32_t get_height() const;
        [[nodiscard]] const std::vector<std::unique_ptr<static_object>>& get_static_objects() const;
        [[nodiscard]] const std::vector<std::unique_ptr<entity>>& get_entities() const;
        [[nodiscard]] const texture* get_background_texture() const;

    private:
        uint32_t m_width = 0;
        uint32_t m_height = 0;

        const texture* m_bg_texture = nullptr;
        std::vector<std::unique_ptr<static_object>> m_tiles; // grid of size width * height (null means empty)
        std::vector<std::unique_ptr<entity>> m_entities;
    };
}
