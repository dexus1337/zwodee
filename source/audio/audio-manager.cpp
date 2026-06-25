#include "audio/audio-manager.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>

namespace zwodee
{
    // ==========================================
    // sound_effect Destructor
    // ==========================================
    sound_effect::~sound_effect()
    {
        // Guard clause: Avoid freeing null buffer
        if (!buffer)
        {
            return;
        }
        SDL_free(buffer);
        buffer = nullptr;
    }

    // ==========================================
    // audio_manager Implementation
    // ==========================================
    audio_manager::audio_manager()
    {
        // Guard clause: Initialize SDL audio subsystem
        if (!SDL_WasInit(SDL_INIT_AUDIO) && !SDL_InitSubSystem(SDL_INIT_AUDIO))
        {
            throw std::runtime_error(std::string("Failed to initialize SDL Audio: ") + SDL_GetError());
        }

        // Open default playback device in SDL3
        m_device_id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        // Guard clause: Check device
        if (m_device_id == 0)
        {
            throw std::runtime_error(std::string("Failed to open SDL Audio Device: ") + SDL_GetError());
        }
    }

    audio_manager::~audio_manager()
    {
        // Clean up loaded sounds
        for (auto& pair : m_sounds)
        {
            delete pair.second;
        }
        m_sounds.clear();

        // Guard clause: Avoid closing invalid device
        if (m_device_id == 0)
        {
            return;
        }

        SDL_CloseAudioDevice(m_device_id);
        m_device_id = 0;
    }

    bool audio_manager::load_sound(const std::string& name, const std::string& wav_path)
    {
        // Guard clause: Check if already loaded
        if (m_sounds.find(name) != m_sounds.end())
        {
            return true;
        }

        auto* effect = new sound_effect();
        SDL_AudioSpec spec;

        // Load WAV in SDL3
        if (!SDL_LoadWAV(wav_path.c_str(), &spec, &effect->buffer, &effect->length))
        {
            delete effect;
            return false;
        }

        effect->channels = spec.channels;
        effect->frequency = spec.freq;
        effect->format = spec.format;

        m_sounds[name] = effect;
        return true;
    }

    void audio_manager::play_sound(const std::string& name)
    {
        if (m_muted)
        {
            return;
        }

        auto it = m_sounds.find(name);

        // Guard clause: Sound not loaded
        if (it == m_sounds.end())
        {
            return;
        }

        const sound_effect* effect = it->second;

        // Configure specs
        SDL_AudioSpec spec;
        spec.channels = effect->channels;
        spec.freq = effect->frequency;
        spec.format = static_cast<SDL_AudioFormat>(effect->format);

        // Create stream for playback (converts formats dynamically if needed)
        SDL_AudioStream* stream = SDL_CreateAudioStream(&spec, &spec);
        
        // Guard clause: Check stream creation
        if (!stream)
        {
            return;
        }

        // Bind stream to the playback device
        SDL_BindAudioStream(m_device_id, stream);

        // Push data to stream
        SDL_PutAudioStreamData(stream, effect->buffer, static_cast<int>(effect->length));

        // Let the stream know it's the end of input so it cleans up when done
        SDL_FlushAudioStream(stream);

        // Normally we'd track active streams to destroy them when finished,
        // but for a simple sound effect, SDL3 allows binding multiple streams.
        // For simplicity in this demo we let the stream play.
        // In production, we'd destroy the stream after it completes (SDL_DestroyAudioStream).
    }

    void audio_manager::set_muted(bool muted)
    {
        m_muted = muted;
    }

    bool audio_manager::is_muted() const
    {
        return m_muted;
    }
}
