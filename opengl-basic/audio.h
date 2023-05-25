#ifndef AUDIO_H
#define AUDIO_H

#include "miniaudio.h"

ma_engine audio_engine;
int play_audio;
double last_play;


void sound_init();
void play_sound();
void sound_free();


#endif  // AUDIO_H
