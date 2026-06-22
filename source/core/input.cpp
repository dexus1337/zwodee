#include "core/input.hpp"
#include <cstring>

namespace zwodee
{
    bool input_state::is_down(button_mask button) const
    {
        return (buttons & button) != 0;
    }

    void input_state::serialize(std::vector<uint8_t>& buffer) const
    {
        // Add serialized state of buttons to buffer
        size_t size = sizeof(buttons);
        size_t original_size = buffer.size();
        buffer.resize(original_size + size);
        std::memcpy(buffer.data() + original_size, &buttons, size);
    }

    void input_state::deserialize(const std::vector<uint8_t>& buffer, size_t& offset)
    {
        // Guard clause: Ensure we don't read past the buffer bounds
        if (offset + sizeof(buttons) > buffer.size())
        {
            return;
        }

        std::memcpy(&buttons, buffer.data() + offset, sizeof(buttons));
        offset += sizeof(buttons);
    }
}
