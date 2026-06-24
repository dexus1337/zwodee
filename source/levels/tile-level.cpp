#include "levels/tile-level.hpp"
#include "entities/static-object.hpp"
#include "entities/entity-player.hpp"
#include "entities/entity.hpp"
#include "graphics/renderer.hpp"
#include "graphics/texture.hpp"

namespace zwodee
{
    tile_level::tile_level(uint32_t width, uint32_t height)
        : m_width(width), m_height(height)
    {
        m_tiles.resize(width * height);
    }

    tile_level::~tile_level()
    {
        m_tiles.clear();
        m_entities.clear();
    }

    void tile_level::clear_level()
    {
        m_tiles.clear();
        m_tiles.resize(m_width * m_height);
        m_entities.clear();
    }

    void tile_level::on_enter()
    {
    }

    void tile_level::on_exit()
    {
    }

    void tile_level::set_player_input(const input_state& input)
    {
        for (auto& ent : m_entities)
        {
            auto* player = dynamic_cast<entity_player*>(ent.get());
            
            // Guard clause: Set input if player entity found
            if (player)
            {
                player->set_input(input);
            }
        }
    }

    void tile_level::tick()
    {
        // Update all active dynamic entities
        for (auto& ent : m_entities)
        {
            // Guard clause: Skip dead entities
            if (ent->is_dead())
            {
                continue;
            }

            ent->tick();
        }
    }

    void tile_level::render(renderer& target_renderer, double alpha)
    {
        // Render background texture stretched over the entire level
        if (m_bg_texture)
        {
            float lvl_w = static_cast<float>(m_width * 32);
            float lvl_h = static_cast<float>(m_height * 32);
            int tex_w = m_bg_texture->get_width();
            int tex_h = m_bg_texture->get_height();

            target_renderer.draw_sprite(*m_bg_texture, 0, 0, tex_w, tex_h, 0.0f, 0.0f, lvl_w, lvl_h);
        }

        // Render static tiles
        for (const auto& tile : m_tiles)
        {
            // Guard clause: Skip empty grid spaces
            if (!tile)
            {
                continue;
            }

            tile->render(target_renderer, alpha);
        }

        // Render dynamic entities
        for (const auto& ent : m_entities)
        {
            // Guard clause: Skip dead entities
            if (ent->is_dead())
            {
                continue;
            }

            ent->render(target_renderer, alpha);
        }
    }

    render_snapshot tile_level::get_render_snapshot(int display_w, int display_h) const
    {
        render_snapshot snapshot;
        snapshot.reserve(m_tiles.size() + m_entities.size());

        float offset_x = 0.0f;
        float offset_y = 0.0f;

        float level_pixel_width = static_cast<float>(m_width * 32);
        float level_pixel_height = static_cast<float>(m_height * 32);

        // Center level horizontally if it is smaller than display width
        if (level_pixel_width < static_cast<float>(display_w))
        {
            offset_x = (static_cast<float>(display_w) - level_pixel_width) / 2.0f;
        }
        // Center level vertically if it is smaller than display height
        if (level_pixel_height < static_cast<float>(display_h))
        {
            offset_y = (static_cast<float>(display_h) - level_pixel_height) / 2.0f;
        }

        // Gather background texture stretched over the entire level
        if (m_bg_texture)
        {
            float lvl_w = static_cast<float>(m_width * 32);
            float lvl_h = static_cast<float>(m_height * 32);
            int tex_w = m_bg_texture->get_width();
            int tex_h = m_bg_texture->get_height();

            render_node node;
            node.x = offset_x;
            node.y = offset_y;
            node.w = lvl_w;
            node.h = lvl_h;
            node.tex = m_bg_texture;
            node.src_x = 0;
            node.src_y = 0;
            node.src_w = tex_w;
            node.src_h = tex_h;
            snapshot.push_back(node);
        }

        // Gather static tiles
        for (const auto& tile : m_tiles)
        {
            if (tile)
            {
                render_node node = tile->get_render_node();
                node.x += offset_x;
                node.y += offset_y;
                snapshot.push_back(node);
            }
        }

        // Gather dynamic entities
        for (const auto& ent : m_entities)
        {
            if (ent && !ent->is_dead())
            {
                render_node node = ent->get_render_node();
                node.x += offset_x;
                node.y += offset_y;
                snapshot.push_back(node);
            }
        }

        return snapshot;
    }

    void tile_level::set_tile(uint32_t x, uint32_t y, uint16_t tile_id, uint8_t flags, const texture* tex)
    {
        // Guard clause: Check bounds
        if (x >= m_width || y >= m_height)
        {
            return;
        }

        // Guard clause: empty tile removal
        if (tile_id == 0)
        {
            m_tiles[y * m_width + x].reset();
            return;
        }

        // Static objects are 32x32 tiles by default
        auto obj = std::make_unique<static_object>(0, tex);
        obj->set_position(static_cast<float>(x * 32), static_cast<float>(y * 32));
        obj->set_size(32.0f, 32.0f);
        
        m_tiles[y * m_width + x] = std::move(obj);
    }

    void tile_level::add_entity(std::unique_ptr<entity> ent)
    {
        // Guard clause: Skip null entity
        if (!ent)
        {
            return;
        }

        m_entities.push_back(std::move(ent));
    }

    uint32_t tile_level::get_width() const
    {
        return m_width;
    }

    uint32_t tile_level::get_height() const
    {
        return m_height;
    }

    const std::vector<std::unique_ptr<static_object>>& tile_level::get_static_objects() const
    {
        return m_tiles;
    }

    const std::vector<std::unique_ptr<entity>>& tile_level::get_entities() const
    {
        return m_entities;
    }

    void tile_level::set_background_texture(const texture* tex)
    {
        m_bg_texture = tex;
    }

    const texture* tile_level::get_background_texture() const
    {
        return m_bg_texture;
    }
}
