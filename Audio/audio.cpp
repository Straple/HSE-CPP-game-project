#include <iostream>
#include "audio.hpp"
#include "../Game/Game_objects/game_variables.hpp"


namespace Audio {
    efloat global_volume=1, sound_volume=80, music_volume=1, UI_volume;
		void sound::play() {
			for (size_t i = 0; i < size; i++) {
				if (!sound_ptr[i]->isPlaying()) {
					sound_ptr[i]->setVolume(sound_volume * global_volume);

					sound_ptr[i]->play();

					break;
				}
			}
		}
		void sound::stop() {
			for (size_t i = 0; i < size; i++) {
				sound_ptr[i]->stop();
			}
		}

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

		bool sound::is_play() {
			size_t i;
			for (i = 0; i < size && sound_ptr[i]->isPlaying() == false; i++) {
			}
			return i < size;
		}

		void sound::OpenSound(audiere::AudioDevicePtr& device, const char* path, bool streaming, size_t _size) {
			size = _size;
			sound_ptr = new audiere::OutputStreamPtr[_size];

			for (size_t i = 0; i < _size; i++) {
				sound_ptr[i] = audiere::OpenSound(device, path, streaming);
                sound_ptr[i]->setRepeat(false);
			}
		}



		inline void UI_sound::play() {
            for (size_t i = 0; i < size; i++) {
				if (!sound_ptr[i]->isPlaying()) {
					sound_ptr[i]->setVolume(UI_volume * global_volume);
					sound_ptr[i]->play();
					break;
				}
			}
		}
		void UI_sound::OpenSound(audiere::AudioDevicePtr &device, const char *path, bool streaming, size_t _size) {
            size = _size;
			sound_ptr = new audiere::OutputStreamPtr[_size];
			for (size_t i = 0; i < _size; i++) {
				sound_ptr[i] = audiere::OpenSound(device, path, streaming);
			}
		}

		void music::update(float dt) {
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
			}
			else if (sound_cond == SC_up) {
				efloat Volume;
                if (music_volume < sound_ptr->getVolume() + dt * .3f) {
                    Volume = music_volume;
                }
                else {
                    Volume = sound_ptr->getVolume() + dt * .3f;
                }
				sound_ptr->setVolume(Volume);
				if (Volume == music_volume) {
					sound_cond = SC_play;
				}
			}
		}



	void init_musics(audiere::AudioDevicePtr& device, std::vector<std::string> Name) {
		size_t index = 0;
		for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
			game_variables::Musics[index].sound_ptr = OpenSound(device, ("Audio\\Musics\\" + *it_name).c_str(), true);
			index++;
		}
	}
	void init_sounds(audiere::AudioDevicePtr& device, std::vector<std::string> Name) {
		size_t index = 0;
		for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
			game_variables::Sounds[index].OpenSound(device, ("Audio\\Sounds\\" + *it_name).c_str(), true, 15);
			index++;
		}
	}
	void init_UI(audiere::AudioDevicePtr& device, std::vector<std::string> Name) {
		size_t index = 0;
		for (auto it_name = Name.begin(); it_name != Name.end(); it_name++) {
			game_variables::UI[index].OpenSound(device, ("Audio\\Sounds\\" + *it_name).c_str(), true, 15);
			index++;
		}
	}

	void init_audio(audiere::AudioDevicePtr& device) {
		init_musics(device, { "Dramatic_Battle.mp3" });
		init_sounds(device, { "sniper_sound.mp3", "blaster.mp3", "magic.mp3", "rifle_shot.mp3", "slime_eating.wav", "explosion.mp3", "run.mp3",
                                                                                                                                     "slime_shot.mp3"});
//		init_UI(device, { "UI.mp3" });
	}

	void play_music(music_type mus) {
		game_variables::Musics[mus].sound_cond = SC_up;
        game_variables::Musics[mus].sound_ptr->setVolume(0.5f);
        game_variables::Musics[mus].sound_ptr->play();
        game_variables::Musics[mus].sound_ptr->setRepeat(true);
	}
	void stop_music(music_type mus) {
        game_variables::Musics[mus].sound_cond = SC_down;
	}

	void change_of_music(music_type _old, music_type _new) {
		stop_music(_old);
		play_music(_new);
	}

	void update_musics(float dt) {
		for (int i = 0; i < MT_COUNT; i++) {
            game_variables::Musics[i].update(dt);
		}
	}

	void update_musics_volume() {
		for (int i = 0; i < MT_COUNT; i++) {
            game_variables::Musics[i].sound_ptr->setVolume(music_volume * global_volume);
		}
	}
}