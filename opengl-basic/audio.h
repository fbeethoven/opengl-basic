#ifndef AUDIO_H
#define AUDIO_H

#include "miniaudio.h"


extern int play_audio;
extern double last_play;

void sound_init();
void play_sound();
void sound_free();


#endif  // AUDIO_H
