#pragma once

/**
 * @file    audio-manager.hpp
 * @author  dexus1337
 * @brief   Defines the audio_manager class wrapping SDL3 audio functions.
 * @version 1.0
 * @date    21.06.2026
 */

#include "api/api.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

// Forward declaration of SDL structures
typedef uint32_t SDL_AudioDeviceID;

namespace zwodee
{
    /**
     * @struct  sound_effect
     * @brief   Holds raw audio buffer data for playback.
     */
    struct ZWODEE_API sound_effect
    {
        uint8_t* buffer = nullptr;
        uint32_t length = 0;
        // Audio spec parameters
        int channels = 0;
        int frequency = 0;
        uint32_t format = 0; // SDL_AudioFormat

        ~sound_effect();
    };

    /**
     * @class   audio_manager
     * @brief   An OOP manager class for playing sound effects and audio tracks in SDL3.
     */
    class ZWODEE_API audio_manager
    {
    public:
        audio_manager();
        ~audio_manager();

        // Prevent copying
        audio_manager(const audio_manager&) = delete;
        audio_manager& operator=(const audio_manager&) = delete;

        /**
         * @brief Plays a loaded sound effect.
         */
        void play_sound(const std::string& name);

        void set_muted(bool muted);
        [[nodiscard]] bool is_muted() const;

        /**
         * @brief Loads a sound effect from a WAV file.
         */
        bool load_sound(const std::string& name, const std::string& wav_path);

    private:
        SDL_AudioDeviceID m_device_id = 0;
        std::unordered_map<std::string, sound_effect*> m_sounds;
        bool m_muted = false;
    };
}
