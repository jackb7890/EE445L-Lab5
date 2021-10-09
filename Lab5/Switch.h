#pragma once

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"							
#include "../inc/Timer0A.h"
#include "../inc/Timer1A.h"
#include "Song.h"


void rewind(Song_t* song);

void playPause(uint8_t* pausedFlag);

void cycleTempo(Song_t* song, uint8_t* tempos);

