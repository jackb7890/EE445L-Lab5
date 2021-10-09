#include "Switch.h"

void rewind(Song_t* song)
{
	DisableInterrupts();
	song->currNote = 0;
	TIMER1_TAILR_R = song->freqs[song->currNote];
	TIMER0_TAILR_R = song->durations[song->currNote];
	EnableInterrupts();
}

void playPause(uint8_t* pausedFlag)
{
	if (*pausedFlag == 0)
	{
		*pausedFlag = 1;
	}
	else
	{
		*pausedFlag = 0;
	}
}

void cycleTempo(Song_t* song, uint8_t* tempos)
{
	song->tempoIdx = (song->tempoIdx + 1 ) % 3;
	song->tempo = tempos[song->tempoIdx];
}
