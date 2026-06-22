#include "entities/animator.hpp"

namespace zwodee
{
    animator::animator(int total_frames, int ticks_per_frame, bool loops)
        : m_total_frames(total_frames), m_ticks_per_frame(ticks_per_frame), m_loops(loops)
    {
        // Guard clause: Enforce minimum valid values
        if (m_total_frames < 1)
        {
            m_total_frames = 1;
        }
        if (m_ticks_per_frame < 1)
        {
            m_ticks_per_frame = 1;
        }
    }

    void animator::update(int ticks)
    {
        m_accumulated_ticks += ticks;

        // Guard clause: Not enough ticks to advance
        if (m_accumulated_ticks < m_ticks_per_frame)
        {
            return;
        }

        int frame_advances = m_accumulated_ticks / m_ticks_per_frame;
        m_accumulated_ticks %= m_ticks_per_frame;
        m_current_frame += frame_advances;

        // Guard clause: Frame within valid bounds
        if (m_current_frame < m_total_frames)
        {
            return;
        }

        // Handle loop wrap
        if (m_loops)
        {
            m_current_frame %= m_total_frames;
            return;
        }

        // Clamp to final frame
        m_current_frame = m_total_frames - 1;
    }

    void animator::reset()
    {
        m_current_frame = 0;
        m_accumulated_ticks = 0;
    }

    int animator::get_current_frame() const
    {
        return m_current_frame;
    }

    void animator::set_animation(int total_frames, int ticks_per_frame, bool loops)
    {
        m_total_frames = total_frames < 1 ? 1 : total_frames;
        m_ticks_per_frame = ticks_per_frame < 1 ? 1 : ticks_per_frame;
        m_loops = loops;
        reset();
    }
}
