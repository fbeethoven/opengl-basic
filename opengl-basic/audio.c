#include "audio.h"


#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


void sound_init() {
     ma_result result;

     result = ma_engine_init(NULL, &audio_engine);
     if (result != MA_SUCCESS) {
         return;
     }

     play_audio = 0;
}


void play_sound() {
    if (play_audio == 0) {
        play_audio = 1;
        ma_engine_play_sound(&audio_engine, "assets/sounds/test.wav", NULL);
    }
}

void sound_free() {
    ma_engine_uninit(&audio_engine);
}
