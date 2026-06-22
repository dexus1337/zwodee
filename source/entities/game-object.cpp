#include "entities/game-object.hpp"
#include "graphics/render-snapshot.hpp"
#include "core/input.hpp"
#include <cstring>

namespace zwodee
{
    game_object::game_object(uint32_t network_id)
        : m_network_id(network_id)
    {
    }

    void game_object::tick()
    {
        m_x += m_vx;
        m_y += m_vy;
    }

    void game_object::render(renderer& target_renderer, double alpha)
    {
        // Base implementation does nothing
        (void)target_renderer;
        (void)alpha;
    }

    render_node game_object::get_render_node() const
    {
        // Empty texture representation for base objects
        return render_node{ m_x, m_y, m_width, m_height, nullptr, 0, 0, 0, 0 };
    }

    void game_object::serialize(std::vector<uint8_t>& buffer) const
    {
        size_t size = sizeof(m_network_id) + sizeof(m_x) + sizeof(m_y) + sizeof(m_vx) + sizeof(m_vy) + sizeof(m_width) + sizeof(m_height);
        size_t original_size = buffer.size();
        buffer.resize(original_size + size);

        uint8_t* ptr = buffer.data() + original_size;
        std::memcpy(ptr, &m_network_id, sizeof(m_network_id)); ptr += sizeof(m_network_id);
        std::memcpy(ptr, &m_x, sizeof(m_x)); ptr += sizeof(m_x);
        std::memcpy(ptr, &m_y, sizeof(m_y)); ptr += sizeof(m_y);
        std::memcpy(ptr, &m_vx, sizeof(m_vx)); ptr += sizeof(m_vx);
        std::memcpy(ptr, &m_vy, sizeof(m_vy)); ptr += sizeof(m_vy);
        std::memcpy(ptr, &m_width, sizeof(m_width)); ptr += sizeof(m_width);
        std::memcpy(ptr, &m_height, sizeof(m_height));
    }

    void game_object::deserialize(const std::vector<uint8_t>& buffer, size_t& offset)
    {
        size_t size = sizeof(m_network_id) + sizeof(m_x) + sizeof(m_y) + sizeof(m_vx) + sizeof(m_vy) + sizeof(m_width) + sizeof(m_height);
        
        // Guard clause: Verify enough bytes
        if (offset + size > buffer.size())
        {
            return;
        }

        const uint8_t* ptr = buffer.data() + offset;
        std::memcpy(&m_network_id, ptr, sizeof(m_network_id)); ptr += sizeof(m_network_id);
        std::memcpy(&m_x, ptr, sizeof(m_x)); ptr += sizeof(m_x);
        std::memcpy(&m_y, ptr, sizeof(m_y)); ptr += sizeof(m_y);
        std::memcpy(&m_vx, ptr, sizeof(m_vx)); ptr += sizeof(m_vx);
        std::memcpy(&m_vy, ptr, sizeof(m_vy)); ptr += sizeof(m_vy);
        std::memcpy(&m_width, ptr, sizeof(m_width)); ptr += sizeof(m_width);
        std::memcpy(&m_height, ptr, sizeof(m_height));
        offset += size;
    }

    uint32_t game_object::get_network_id() const
    {
        return m_network_id;
    }

    float game_object::get_x() const
    {
        return m_x;
    }

    float game_object::get_y() const
    {
        return m_y;
    }

    float game_object::get_width() const
    {
        return m_width;
    }

    float game_object::get_height() const
    {
        return m_height;
    }

    void game_object::set_position(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    void game_object::set_grid_position(uint32_t gx, uint32_t gy)
    {
        constexpr float tile_size = 32.0f;
        m_x = static_cast<float>(gx) * tile_size;
        m_y = static_cast<float>(gy) * tile_size;
    }

    void game_object::set_velocity(float vx, float vy)
    {
        m_vx = vx;
        m_vy = vy;
    }

    void game_object::set_size(float width, float height)
    {
        m_width = width;
        m_height = height;
        m_collision_width = width;
        m_collision_height = height;
    }

    bool game_object::is_collidable() const
    {
        return m_is_collidable;
    }

    void game_object::set_collidable(bool collidable)
    {
        m_is_collidable = collidable;
    }

    float game_object::get_collision_offset_x() const
    {
        return m_collision_offset_x;
    }

    float game_object::get_collision_offset_y() const
    {
        return m_collision_offset_y;
    }

    float game_object::get_collision_width() const
    {
        return m_collision_width;
    }

    float game_object::get_collision_height() const
    {
        return m_collision_height;
    }

    void game_object::set_collision_box(float offset_x, float offset_y, float width, float height)
    {
        m_collision_offset_x = offset_x;
        m_collision_offset_y = offset_y;
        m_collision_width = width;
        m_collision_height = height;
    }

    bool game_object::collides_with(const game_object& other) const
    {
        if (!m_is_collidable || !other.m_is_collidable)
        {
            return false;
        }

        float self_left = m_x + m_collision_offset_x;
        float self_right = self_left + m_collision_width;
        float self_top = m_y + m_collision_offset_y;
        float self_bottom = self_top + m_collision_height;

        float other_left = other.m_x + other.m_collision_offset_x;
        float other_right = other_left + other.m_collision_width;
        float other_top = other.m_y + other.m_collision_offset_y;
        float other_bottom = other_top + other.m_collision_height;

        return (self_left < other_right && self_right > other_left &&
                self_top < other_bottom && self_bottom > other_top);
    }

    void game_object::on_collision(game_object& other)
    {
        (void)other;
    }
}
