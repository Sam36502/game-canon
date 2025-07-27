#ifndef BEEP_H
#define BEEP_H

//	Simple sound synthesis lib
//
#include <raylib.h>
#include <stdlib.h>
#include <stdint.h>

#define BEEP_SAMPLE_FREQ 44100	// Hz
#define BEEP_ENV_TIME 0.020f	// s
#define BEEP_WAVE_LEN 16		// Samples


// Type Definitions

typedef enum {
	WAV_SQUARE,
	WAV_TRIANGLE,
	WAV_SAWTOOTH,
	WAV_SINE,
} SND_Preset;


//	Wave sample data
extern uint8_t SND_wave_data[BEEP_WAVE_LEN];


//	Initialise Sound System
//	
void SND_init();

//	Terminate Sound System
//	
void SND_terminate();


//	Load a preset wave into the wavetable
//	
void SND_loadpreset(SND_Preset preset);

//	Play a beep sound
//
void SND_beep(float freq, float duration);

#endif
