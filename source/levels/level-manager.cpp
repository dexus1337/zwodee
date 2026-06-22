#include "levels/level-manager.hpp"
#include "levels/level.hpp"
#include "core/input.hpp"

namespace zwodee
{
    level_manager::level_manager() = default;

    level_manager::~level_manager()
    {
        if (m_active_level)
        {
            m_active_level->on_exit();
        }
        m_levels.clear();
        m_active_level = nullptr;
    }

    void level_manager::register_level(const std::string& name, std::unique_ptr<level> lvl)
    {
        // Guard clause: Avoid registering null level
        if (!lvl)
        {
            return;
        }

        m_levels[name] = std::move(lvl);
    }

    void level_manager::transition_to(const std::string& name)
    {
        auto it = m_levels.find(name);

        // Guard clause: Level not found
        if (it == m_levels.end())
        {
            return;
        }

        // Trigger exit callback on the current active level if it exists
        if (m_active_level)
        {
            m_active_level->on_exit();
        }

        m_active_level = it->second.get();
        m_active_level_name = name;
        
        m_active_level->on_enter();
    }

    void level_manager::set_player_input(const input_state& input)
    {
        // Guard clause: No level active
        if (!m_active_level)
        {
            return;
        }

        m_active_level->set_player_input(input);
    }

    void level_manager::tick()
    {
        // Guard clause: No level active
        if (!m_active_level)
        {
            return;
        }

        m_active_level->tick();
    }

    void level_manager::render(renderer& target_renderer, double alpha)
    {
        // Guard clause: No level active
        if (!m_active_level)
        {
            return;
        }

        m_active_level->render(target_renderer, alpha);
    }

    render_snapshot level_manager::get_active_level_snapshot(int display_w, int display_h) const
    {
        if (!m_active_level)
        {
            return render_snapshot();
        }
        return m_active_level->get_render_snapshot(display_w, display_h);
    }

    std::string level_manager::get_active_level_name() const
    {
        return m_active_level_name;
    }
}
