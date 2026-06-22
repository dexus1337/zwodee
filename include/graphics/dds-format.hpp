#pragma once

/**
 * @file    dds-format.hpp
 * @author  dexus1337
 * @brief   Defines DDS (DirectDraw Surface) header structures.
 * @version 1.0
 * @date    22.06.2026
 */

#include <cstdint>

namespace zwodee
{
    #pragma pack(push, 1)

    struct dds_pixel_format
    {
        uint32_t size;
        uint32_t flags;
        uint32_t four_cc;
        uint32_t rgb_bit_count;
        uint32_t r_bit_mask;
        uint32_t g_bit_mask;
        uint32_t b_bit_mask;
        uint32_t a_bit_mask;
    };

    struct dds_header
    {
        uint32_t magic; // "DDS "
        uint32_t size;
        uint32_t flags;
        uint32_t height;
        uint32_t width;
        uint32_t pitch_or_linear_size;
        uint32_t depth;
        uint32_t mip_map_count;
        uint32_t reserved1[11];
        dds_pixel_format ddspf;
        uint32_t caps;
        uint32_t caps2;
        uint32_t caps3;
        uint32_t caps4;
        uint32_t reserved2;
    };

    #pragma pack(pop)
}
