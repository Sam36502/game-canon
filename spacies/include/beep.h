#ifndef BEEP_H
#define BEEP_H

//
//	Beep.h
//	Simple sound synthesis using raylib
//
//	Produces notes using a 16-step, 8-bit wavetable.
//	Includes a very basic attack/release envelope to reduce popping
//

#include <raylib.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define BEEP_SAMPLE_FREQ 44100	// Hz
#define BEEP_FREQ_A4 440.0		// Hz
#define BEEP_SEMITONE 1.0594630943593	// Ratio
#define BEEP_ENV_TIME 0.020f	// s
#define BEEP_WAVE_LEN 16		// Samples


//	
//	Type Definitions
//	

typedef enum {
	WAV_SQUARE,
	WAV_TRIANGLE,
	WAV_SAWTOOTH,
	WAV_SINE,
	WAV_ORGAN,
} SND_Preset;


//	
//	Global Variable Declarations
//	

//	Wave sample data
extern uint8_t SND_wave_data[BEEP_WAVE_LEN];

extern float SND_frequency;
extern float SND_duration;
extern float SND_volume;


//	
//	Function Declarations
//	

//	Initialise Sound System
//	
void SND_init();

//	Terminate Sound System
//	
void SND_terminate();

//	Load a preset wave into the wavetable
//	
void SND_loadpreset(SND_Preset preset);

//	Converts a midi note-number to a frequency
//
float SND_note_to_freq(uint8_t note);

//	Play a beep sound for a given duration
//
void SND_beep(float freq, float duration);

//	Draw an Oscilloscope with the last few samples
//
void SND_draw_osc(Rectangle scope);


#endif
