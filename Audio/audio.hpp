#ifndef AUDIO_HPP
#define AUDIO_HPP
#include "../Objects/Geometry/geometry_utils.hpp"
#include "audiere.h"

namespace Audio {

enum sound_type : uint8_t {
    ST_sniper_shot,
    ST_golden_gun_shot,
    ST_staff_spell,
    ST_rifle_shot,
    ST_slime_eating,
    ST_boom,
    ST_run,
    ST_slime_shot,

    ST_COUNT,
};

enum UI_sound_type : uint8_t {
    UI_focus,

    UI_COUNT,
};

enum music_type : uint8_t {
    MT_gameplay,

    MT_COUNT
};

enum music_condition : uint8_t {
    SC_stop,  //
    SC_down,  //
    SC_up,    //
    SC_play   //
};

struct sound {
    audiere::OutputStreamPtr *sound_ptr;
    size_t size;

    void play();
    void stop();

    /*void repeat() {
        for (int i = 0; i < size; i++) {
            sound_ptr[i]->setRepeat(true);
        }
    }*/
    /*void no_repeat() {
        for (int i = 0; i < size; i++) {
            sound_ptr[i]->setRepeat(false);
        }
    }*/

    bool is_play();

    void OpenSound(audiere::AudioDevicePtr &device, const char *path, bool streaming, size_t _size);
};

struct UI_sound {
    audiere::OutputStreamPtr *sound_ptr;
    size_t size;

    inline void play();
    void OpenSound(audiere::AudioDevicePtr &device, const char *path, bool streaming, size_t _size);
};

struct music {
    audiere::OutputStreamPtr sound_ptr;
    music_condition sound_cond;

    void update(float dt);
};

void init_musics(audiere::AudioDevicePtr &device, std::vector<std::string> Name);
void init_sounds(audiere::AudioDevicePtr &device, std::vector<std::string> Name);
void init_UI(audiere::AudioDevicePtr &device, std::vector<std::string> Name);

void init_audio(audiere::AudioDevicePtr &device);

void play_music(music_type mus);
void stop_music(music_type mus);

void change_of_music(music_type _old, music_type _new);

void update_musics(float dt);

void update_musics_volume();

void play_sound(sound_type type);

}  // namespace Audio

struct audio_variables {
    static inline Audio::sound Sounds[Audio::ST_COUNT];
    static inline Audio::music Musics[Audio::MT_COUNT];
    static inline Audio::UI_sound UI[Audio::UI_COUNT];
    static inline std::vector<Audio::sound_type> SoundsQueue;
};

#endif  // AUDIO_HPP