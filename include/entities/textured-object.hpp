#pragma once

/**
 * @file    textured-object.hpp
 * @author  dexus1337
 * @brief   Defines the textured_object class, extending game_object to support sprite rendering.
 * @version 1.0
 * @date    21.06.2026
 */

#include "entities/game-object.hpp"

namespace zwodee
{
    class texture;

    /**
     * @class   textured_object
     * @brief   A game object that has a visual sprite texture.
     */
    class ZWODEE_API textured_object : public game_object
    {
    public:
        /**
         * @brief Constructs a new textured object.
         */
        textured_object(uint32_t network_id, const texture* tex);

        /**
         * @brief Renders the texture to screen.
         */
        void render(renderer& target_renderer, double alpha) override;

        /**
         * @brief Extracts a lightweight render snapshot description node (lock-free).
         */
        render_node get_render_node() const override;

        inline const texture* get_texture() const         { return m_texture; }
        inline bool           get_flip_horizontal() const { return m_flip_horizontal; }

        inline void set_texture(const texture* tex) { m_texture = tex; }
        inline void set_flip_horizontal(bool flip)  { m_flip_horizontal = flip; }

    protected:
        const texture* m_texture = nullptr;
        bool m_flip_horizontal = false;
    };
}
