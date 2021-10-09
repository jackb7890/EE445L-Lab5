#pragma once

#include <stdint.h>

#define A 5682
#define B 5061
#define C 4789
#define D 4256
#define E 3792
#define F 3582
#define G 3189
#define HIGHA 2841
#define REST 0xFFFFFFFF

#define quarter 80000000
#define eighth 40000000
#define triplet 26666667
#define half 160000000
#define whole 320000000

#define NOTESPACE 1500000

typedef struct Song
{
	uint32_t currNote;
	uint8_t tempo;
	uint32_t tempoIdx;
	uint32_t freqs[57];
	uint32_t durations[57];
} Song_t;

void song_init(Song_t* song, uint32_t* freqs, uint32_t freqSize, uint32_t* durs, uint32_t durSize);
