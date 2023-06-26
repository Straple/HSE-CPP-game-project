#include "audio.hpp"
#include "../utils.hpp"

namespace Audio {
efloat global_volume = 1, sound_volume = 1, music_volume = 0.3, UI_volume;

void sound::play() {
#ifdef AUDIERE
    for (size_t i = 0; i < size; i++) {
        if (!sound_ptr[i]->isPlaying()) {
            sound_ptr[i]->setVolume(sound_volume * global_volume);

            sound_ptr[i]->play();

            break;
        }
    }
#endif
}

void sound::stop() {
#ifdef AUDIERE
    for (size_t i = 0; i < size; i++) {
        sound_ptr[i]->stop();
    }
#endif
}

bool sound::is_play() {
    size_t i = 0;
#ifdef AUDIERE
    for (; i < size && !sound_ptr[i]->isPlaying(); i++) {
    }
#endif
    return i < size;
}

#ifdef AUDIERE
void sound::OpenSound(
    audiere::AudioDevicePtr &device,
    const char *path,
    bool streaming,
    size_t _size
) {
    size = _size;
    sound_ptr = new audiere::OutputStreamPtr[_size];

    for (size_t i = 0; i < _size; i++) {
        sound_ptr[i] = audiere::OpenSound(device, path, streaming);
        sound_ptr[i]->setRepeat(false);
    }
}
#endif

inline void UI_sound::play() {
#ifdef AUDIERE
    for (size_t i = 0; i < size; i++) {
        if (!sound_ptr[i]->isPlaying()) {
            sound_ptr[i]->setVolume(UI_volume * global_volume);
            sound_ptr[i]->play();
            break;
        }
    }
#endif
}

#ifdef AUDIERE
void UI_sound::OpenSound(
    audiere::AudioDevicePtr &device,
    const char *path,
    bool streaming,
    size_t _size
) {
    size = _size;
    sound_ptr = new audiere::OutputStreamPtr[_size];
    for (size_t i = 0; i < _size; i++) {
        sound_ptr[i] = audiere::OpenSound(device, path, streaming);
    }
}
#endif

void music::update(float dt) {
#ifdef AUDIERE
    if (sound_cond == SC_down) {
        float Volume = 0;
        if ((sound_ptr->getVolume() - dt * .5f) > 0) {
            Volume = sound_ptr->getVolume() - dt * .5f;
        }
        sound_ptr->setVolume(Volume);
        if (Volume == 0) {
            sound_cond = SC_stop;
            sound_ptr->stop();
        }
    } else if (sound_cond == SC_up) {
        efloat Volume;
        if (music_volume < sound_ptr->getVolume() + dt * .3f) {
            Volume = music_volume;
        } else {
            Volume = sound_ptr->getVolume() + dt * .3f;
        }
        sound_ptr->setVolume(Volume);
        if (Volume == music_volume) {
            sound_cond = SC_play;
        }
    }
#endif
}

#ifdef AUDIERE
void init_musics(
    audiere::AudioDevicePtr &device,
    std::vector<std::string> Name
) {
    size_t index = 0;
    for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
        audio_variables::Musics[index].sound_ptr =
            OpenSound(device, ("Audio\\Musics\\" + *it_name).c_str(), true);
        index++;
    }
}

void init_sounds(
    audiere::AudioDevicePtr &device,
    std::vector<std::string> Name
) {
    size_t index = 0;
    for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
        audio_variables::Sounds[index].OpenSound(
            device, ("Audio\\Sounds\\" + *it_name).c_str(), true, 15
        );
        index++;
    }
}

void init_UI(audiere::AudioDevicePtr &device, std::vector<std::string> Name) {
    size_t index = 0;
    for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
        audio_variables::UI[index].OpenSound(
            device, ("Audio\\Sounds\\" + *it_name).c_str(), true, 15
        );
        index++;
    }
}

void init_audio(audiere::AudioDevicePtr &device) {
    init_musics(device, {"Dramatic_Battle.mp3"});
    init_sounds(
        device,
        {"sniper_sound.mp3", "blaster.mp3", "magic.mp3", "rifle_shot.mp3",
         "slime_eating.wav", "explosion.mp3", "run.mp3", "slime_shot.mp3"}
    );
    //		init_UI(device, { "UI.mp3" });
}

#endif

void play_music(music_type mus) {
#ifdef AUDIERE
    audio_variables::Musics[mus].sound_cond = SC_up;
    audio_variables::Musics[mus].sound_ptr->setVolume(0.5f);
    audio_variables::Musics[mus].sound_ptr->play();
    audio_variables::Musics[mus].sound_ptr->setRepeat(true);
#endif
}

void stop_music(music_type mus) {
    audio_variables::Musics[mus].sound_cond = SC_down;
}

void change_of_music(music_type _old, music_type _new) {
    stop_music(_old);
    play_music(_new);
}

void update_musics(float dt) {
    for (int i = 0; i < MT_COUNT; i++) {
        audio_variables::Musics[i].update(dt);
    }
}

void update_musics_volume() {
#ifdef AUDIERE
    for (int i = 0; i < MT_COUNT; i++) {
        audio_variables::Musics[i].sound_ptr->setVolume(
            music_volume * global_volume
        );
    }
#endif
}

void play_sound(sound_type type) {
    // накапливаем очередь звуков, которые в конце кадра обработаем:
    // server пошлет их клиентам
    // singleplayer проиграет их
    // client проигнорирует, так как получает их от server
    audio_variables::SoundsQueue.push_back(type);

    ASSERT(0 <= type && type < sound_type::ST_COUNT, "wrong sound type");
    // std::cout << "play sound: " << static_cast<int>(type) << '\n';
}

}  // namespace Audio