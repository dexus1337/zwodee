#pragma once

/**
 * @file    game-object.hpp
 * @author  dexus1337
 * @brief   Defines the game_object base class, supporting OOP, serialization and rendering.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"
#include "graphics/render-snapshot.hpp"
#include <cstdint>
#include <vector>

namespace zwodee
{
    class renderer;

    /**
     * @class   game_object
     * @brief   An abstract or base class for all game entities. Fully OOP designed.
     */
    class ZWODEE_API game_object
    {
    public:
        /**
         * @brief Constructs a new game object.
         * @param network_id Unique ID of the object (important for multiplayer matching).
         */
        explicit game_object(uint32_t network_id);

        /**
         * @brief Virtual destructor for OOP safety.
         */
        virtual ~game_object() = default;

        // Prevent copying to avoid object slicing
        game_object(const game_object&) = delete;
        game_object& operator=(const game_object&) = delete;

        /**
         * @brief Updates the game object's logic (runs at 128Hz fixed tick rate).
         */
        virtual void tick();

        /**
         * @brief Renders the game object to screen, optionally using interpolation alpha.
         * @param target_renderer The renderer instance.
         * @param alpha Interpolation value between 0.0 and 1.0.
         */
        virtual void render(renderer& target_renderer, double alpha);

        /**
         * @brief Extracts a lightweight render snapshot description node (lock-free).
         */
        virtual render_node get_render_node() const;

        /**
         * @brief Serializes the object state into a buffer (multiplayer support).
         */
        virtual void serialize(std::vector<uint8_t>& buffer) const;

        /**
         * @brief Deserializes the object state from a buffer.
         */
        virtual void deserialize(const std::vector<uint8_t>& buffer, size_t& offset);

        // Getters and setters
        [[nodiscard]] uint32_t get_network_id() const;
        [[nodiscard]] float get_x() const;
        [[nodiscard]] float get_y() const;
        [[nodiscard]] float get_width() const;
        [[nodiscard]] float get_height() const;

        void set_position(float x, float y);

        /**
         * @brief Positions the object using tile-grid coordinates.
         *        Converts (gx, gy) to pixel coords by multiplying by the tile size (32px).
         */
        void set_grid_position(uint32_t gx, uint32_t gy);

        void set_velocity(float vx, float vy);
        void set_size(float width, float height);

        // Collision API
        [[nodiscard]] bool is_collidable() const;
        void set_collidable(bool collidable);
        [[nodiscard]] float get_collision_offset_x() const;
        [[nodiscard]] float get_collision_offset_y() const;
        [[nodiscard]] float get_collision_width() const;
        [[nodiscard]] float get_collision_height() const;
        void set_collision_box(float offset_x, float offset_y, float width, float height);
        [[nodiscard]] bool collides_with(const game_object& other) const;
        virtual void on_collision(game_object& other);

    protected:
        uint32_t m_network_id = 0;
        float m_x = 0.0f;
        float m_y = 0.0f;
        float m_vx = 0.0f;
        float m_vy = 0.0f;
        float m_width = 32.0f;
        float m_height = 32.0f;

        // Collision properties
        bool m_is_collidable = true;
        float m_collision_offset_x = 0.0f;
        float m_collision_offset_y = 0.0f;
        float m_collision_width = 32.0f;
        float m_collision_height = 32.0f;
    };
}
