#pragma once

/**
 * @file    renderer.hpp
 * @author  dexus1337
 * @brief   Defines the renderer and texture classes, wrapping SDL_Renderer and SDL_Texture.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"
#include <string>
#include <memory>

// Forward declarations
struct SDL_Renderer;

namespace zwodee 
{ 
    class window; 
    class texture;

    /**
     * @class   renderer
     * @brief   Wraps the SDL_Renderer for rendering 2D sprites and geometry.
     */
    class ZWODEE_API renderer
    {
    public:
        /**
         * @brief Constructs a new renderer associated with the given window.
         */
        explicit renderer(const window& target_window, bool vsync);

        /**
         * @brief Destroys the renderer and releases SDL resources.
         */
        ~renderer();

        // Prevent copying
        renderer(const renderer&) = delete;
        renderer& operator=(const renderer&) = delete;

        /**
         * @brief Clears the screen with the current clear color.
         */
        void clear();

        /**
         * @brief Presents the backbuffer to the screen.
         */
        void present();

        /**
         * @brief Sets the clear color of the screen.
         */
        void set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

        /**
         * @brief Set vsync at runtime.
         */
        void set_vsync(bool enable);

        /**
         * @brief Loads a texture from a BMP image file (BMP is natively supported in core SDL3).
         */
        std::unique_ptr<texture> load_texture(const std::string& file_path);

        /**
         * @brief Loads a texture from a custom compiled DDS file.
         */
        std::unique_ptr<texture> load_dds_texture(const std::string& file_path);

        /**
         * @brief Draws a full texture to the screen at coordinates.
         */
        void draw_texture(const texture& tex, float x, float y);

        /**
         * @brief Draws a portion of a texture (sprite sheet) to the screen.
         */
        void draw_sprite(const texture& tex, int src_x, int src_y, int src_w, int src_h, float dest_x, float dest_y, float dest_w, float dest_h);

        /**
         * @brief Gets the raw SDL_Renderer pointer.
         */
        [[nodiscard]] SDL_Renderer* get_raw_renderer() const;

    private:
        SDL_Renderer* m_sdl_renderer = nullptr;
    };
}
