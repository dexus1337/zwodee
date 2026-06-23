#pragma once

/**
 * @file    render-snapshot.hpp
 * @author  dexus1337
 * @brief   Defines lightweight structures for lock-free double-buffered rendering.
 * @version 1.0
 * @date    22.06.2026
 */

#include "api/api.hpp"
#include <vector>

namespace zwodee
{
    class texture;

    /**
     * @struct  render_node
     * @brief   Lightweight copy of game object render status.
     */
    struct render_node
    {
        float x;
        float y;
        float w;
        float h;
        const texture* tex;
        int src_x;
        int src_y;
        int src_w;
        int src_h;
        bool flip_horizontal = false;
        bool flip_vertical = false;
        uint8_t color_mod = 255;
    };

    using render_snapshot = std::vector<render_node>;
}
