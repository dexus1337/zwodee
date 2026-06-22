#include "entities/static-object.hpp"

namespace zwodee
{
    static_object::static_object(uint32_t network_id, const texture* tex)
        : textured_object(network_id, tex)
    {
        m_vx = 0.0f;
        m_vy = 0.0f;
    }

    void static_object::tick()
    {
        // Guard clause: Static objects don't update physics or positions
    }
}
