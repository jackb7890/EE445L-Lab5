#include "Song.h"

void song_init(Song_t* song, uint32_t* freqs, uint32_t freqSize, uint32_t* durs, uint32_t durSize)
{
	
	song->tempo = 1;
	song->tempoIdx = 0;
	song->currNote = 0;
	uint32_t* tempFreqs = freqs;
	uint32_t* tempDurs = durs;
//	for (int x = 0; x < freqSize; x++)
//	{
//		volatile uint32_t temporary = *tempFreqs;
//		song.freqs[x] = *tempFreqs;
//		tempFreqs++;
//	}
	for (uint32_t i = 0; i < durSize; i++)
	{
		song->durations[i] = *tempDurs;
		tempDurs++;
	}
	
	volatile uint32_t i = 0;
	
	while(i < freqSize){
		volatile uint32_t temporary = *tempFreqs;
		song->freqs[i] = *tempFreqs;
		tempFreqs++;
		i++;
	}
	
	

}
