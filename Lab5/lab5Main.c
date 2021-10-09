#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"							
#include "../inc/Timer0A.h"
#include "../inc/Timer1A.h"
#include "../inc/Timer2A.h"
#include "../inc/MAX5353.h"
#include "../inc/LaunchPad.h"
#include "Switch.h"
#include "Song.h"
#include "../inc/PLL.h"

uint8_t tempos[] = {1, 2, 4};


#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration,
                                            // function is controlled by bits
                                            // 1:0 of GPTMTAMR and GPTMTBMR
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_IMR_TATOIM        0x00000001  // GPTM TimerA Time-Out Interrupt
                                            // Mask
#define TIMER_ICR_TATOCINT      0x00000001  // GPTM TimerA Time-Out Raw
                                            // Interrupt
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
                                            // Register Low
																						

Song_t song;
uint32_t sineTable[64] = { 32, 35, 38, 41, 44, 47, 50, 52, 54, 57, 58, 60, 61, 62, 63, 63, 63, 63, 63, 62, 61, 59, 57, 55, 53, 51, 48, 45, 42, 39, 36, 33, 30, 27, 24, 21, 18, 15, 12, 10, 8, 6, 4, 2, 1, 0, 0, 0, 0, 0, 1, 2, 3, 5, 6, 9, 11, 13, 16, 19, 22, 25, 28, 31 };uint32_t sineIdx = 0;
uint8_t finishedNoteFlag = 1;
uint8_t pausedFlag = 0;
	
uint8_t debounceFlag = 0;
	
	
void portAInit(void)
{
  SYSCTL_RCGCGPIO_R |= 0x00000001;
  while((SYSCTL_PRGPIO_R&0x01) == 0){};

  GPIO_PORTA_CR_R = 0x80;
  GPIO_PORTA_DIR_R = 0x00;
  GPIO_PORTA_DEN_R = 0x80;
  GPIO_PORTA_DATA_R = 0;

  GPIO_PORTA_IS_R &= ~0x80; //clear IS bits 0 and 4 to interrupt on edge
  GPIO_PORTA_IBE_R &= ~0x80; //no both edge triggers
  GPIO_PORTA_IEV_R |= 0x80; //set to trigger on rising edge
  GPIO_PORTA_ICR_R |= 0x80; //clear interrupt flags
  GPIO_PORTA_IM_R |= 0x80; //turn on interrupts

  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFFFF1F) | (0x2 << 5); //set priority to 2
  NVIC_EN0_R |= 1; //irq 1
}
	

/* timer0A Task signifying the end of a note */
void timer0ATask(void)
{
//	if (pausedFlag == 0)
//	{
//		if (!finishedNoteFlag)
//		{
//			finishedNoteFlag = 1;
//			song.currNote = ( song.currNote + 1 ) % 57;
//			TIMER1_TAILR_R = song.freqs[song.currNote];
//			TIMER0_TAILR_R = song.durations[song.currNote] / song.tempo;
//		}
//		else
//		{
//			TIMER0_TAILR_R = NOTESPACE;
//			finishedNoteFlag = 0;
//		}
//	}
}

/* timer1A Task specifying the frequency to run through the sine wave */
void timer1ATask(void)
{
	if (!pausedFlag){
		if (finishedNoteFlag)
		{
			DAC_Out(5*sineTable[sineIdx]);
			sineIdx = ( sineIdx + 1 ) % 64;
		}
	}
}

void GPIOPortF_Handler(void)
{
	if ( (GPIO_PORTF_RIS_R & 0x01) == 0x01 ) // SW2 for pause/play
	{
		playPause(&pausedFlag);
		GPIO_PORTF_ICR_R = 0x01;
	}
	else if ( (GPIO_PORTF_RIS_R & 0x10) == 0x10 ) // SW1 for rewind
	{
		rewind(&song);
		GPIO_PORTF_ICR_R = 0x10;
	}
	
	volatile uint8_t checkDebounce = debounceFlag;
	//while (debounceFlag == checkDebounce) {};
	
	volatile int waiting = 0;
	while(waiting < 100){
		waiting ++;
	}
	
}

void GPIOPortA_Handler(void)
{
	volatile uint32_t a = GPIO_PORTA_RIS_R;
	if ( (GPIO_PORTA_RIS_R & 0x80) == 0x80 )
	{
		GPIO_PORTA_ICR_R = 0x80;
		cycleTempo(&song, tempos);
	}
	
	volatile uint8_t checkDebounce = debounceFlag;
	//while (debounceFlag == checkDebounce) {};
	
	volatile int waiting = 0;
	while(waiting < 100){
		waiting ++;
	}
	
}

void timer2ATask(void)
{
	debounceFlag ^= 1;
}

int main()
{
	DisableInterrupts();
	PLL_Init(Bus80MHz);
	uint32_t freqs[57] = { E, E, E, E, D, E, A, E, D, D, REST, REST, D, D, D, D, C, D, G, D, E, D, C, B, C, REST, A, B, C, C, C, C, C, REST, A, B, C, C, C, C, B, C, REST, A, B, C, C, C, C, C, G, C, D, F, HIGHA, F, D };
	uint32_t durations[57] = { quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, eighth, whole, half, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, quarter, triplet, triplet, triplet, quarter, half, quarter, eighth, eighth, quarter, quarter, quarter, quarter, half, quarter, eighth, eighth, quarter, quarter, quarter, quarter, quarter, quarter, quarter, eighth, eighth, quarter, quarter, quarter, quarter, quarter, half, quarter, whole, triplet, triplet, triplet, half };
	song_init(&song, freqs, 57, durations, 57 );
	DAC_Init(sineTable[sineIdx++] / song.tempo);
	LaunchPad_Init();
	Timer0A_Init( timer0ATask, song.durations[song.currNote], 0); //init duration timer
	Timer1A_Init( timer1ATask, song.freqs[song.currNote], 0); //init note frequency timer
	//Timer2A_Init( timer2ATask, 400000, 1); //init note frequency timer
	
	portAInit();
	EnableInterrupts();


	while(1)
	{
		WaitForInterrupt();
	}
}
