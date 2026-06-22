#pragma once

/**
 * @file    level-manager.hpp
 * @author  dexus1337
 * @brief   Defines the level_manager class for handling transitions between different levels.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"
#include "graphics/render-snapshot.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace zwodee
{
    class level;
    class renderer;
    struct input_state;

    /**
     * @class   level_manager
     * @brief   Responsible for active level tracking, loading, updating, and rendering.
     */
    class ZWODEE_API level_manager
    {
    public:
        level_manager();
        ~level_manager();

        // Prevent copying
        level_manager(const level_manager&) = delete;
        level_manager& operator=(const level_manager&) = delete;

        /**
         * @brief Register a level with a name.
         */
        void register_level(const std::string& name, std::unique_ptr<level> lvl);

        /**
         * @brief Switch to a different level.
         */
        void transition_to(const std::string& name);

        /**
         * @brief Set player input for the active level.
         */
        void set_player_input(const input_state& input);

        /**
         * @brief Updates the active level.
         */
        void tick();

        /**
         * @brief Renders the active level.
         */
        void render(renderer& target_renderer, double alpha);

        /**
         * @brief Captured visual snapshot of the active level.
         */
        [[nodiscard]] render_snapshot get_active_level_snapshot(int display_w, int display_h) const;

        /**
         * @brief Gets the active level name.
         */
        [[nodiscard]] std::string get_active_level_name() const;

    private:
        std::unordered_map<std::string, std::unique_ptr<level>> m_levels;
        level* m_active_level = nullptr;
        std::string m_active_level_name;
    };
}
