#include "../include/beep.h"
#include <string.h>

//	Public Definitions
uint8_t SND_wave_data[BEEP_WAVE_LEN] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

//	Preset Wave-Shapes
const static uint8_t SND_wav_square[BEEP_WAVE_LEN] = {
	0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};
const static uint8_t SND_wav_sawtooth[BEEP_WAVE_LEN] = {
	0x10, 0x20, 0x30, 0x40,
	0x50, 0x60, 0x70, 0x80,
	0x90, 0xA0, 0xB0, 0xC0,
	0xD0, 0xE0, 0xF0, 0xFF,
};
const static uint8_t SND_wav_triangle[BEEP_WAVE_LEN] = {
	0x20, 0x40, 0x60, 0x80,
	0xA0, 0xC0, 0xE0, 0xFF,
	0xE0, 0xC0, 0xA0, 0x80,
	0x60, 0x40, 0x20, 0x00,
};
const static uint8_t SND_wav_sine[BEEP_WAVE_LEN] = {
	0x20, 0x40, 0x60, 0x80,
	0xA0, 0xC0, 0xE0, 0xFF,
	0xE0, 0xC0, 0xA0, 0x80,
	0x60, 0x40, 0x20, 0x00,
};


//	Private Definitions
static AudioStream __beep_stream;
static float __beep_duration = 0.0f;
static float __beep_volume = 0.0f;
static float __beep_frequency = 0.0f;
void __callback(void *data, unsigned int size) {
	static float phase = 0.0f;
	for (int i=0; i<size; i++) {

		// Handle basic envelope to avoid popping
		static const float env_delta = 1.0f / (BEEP_ENV_TIME * (float) BEEP_SAMPLE_FREQ);
		if (__beep_duration > BEEP_ENV_TIME && __beep_volume < 1.0f) __beep_volume += env_delta;
		if (__beep_volume > 1.0f) __beep_volume = 1.0f;

		if (__beep_duration <= BEEP_ENV_TIME) __beep_volume -= env_delta;
		if (__beep_volume <= 0.0f) __beep_volume = 0.0f;

		// Create sample from wavetable
		int wi = BEEP_WAVE_LEN * phase;
		float sample = __beep_volume * (float) SND_wave_data[wi];
		((uint8_t *) data)[i] = (uint8_t) sample;

		// Advance wave phase
		if (__beep_frequency > 0.0f) phase += 1.0f / ((float) BEEP_SAMPLE_FREQ / __beep_frequency);
		if (phase > 1.0f) phase -= 1.0f;
	}
	
	__beep_duration -= ((float) size / (float) BEEP_SAMPLE_FREQ);
	if (__beep_duration < 0.0f) __beep_duration = 0.0f;
}

void SND_init() {
	InitAudioDevice();
	SetMasterVolume(0.30f);

	__beep_stream = LoadAudioStream(
		BEEP_SAMPLE_FREQ,
		sizeof(uint8_t) * 8, 1
	);

	SetAudioStreamCallback(__beep_stream, &__callback);
	PlayAudioStream(__beep_stream);
}

void SND_terminate() {
	StopAudioStream(__beep_stream);
	UnloadAudioStream(__beep_stream);

	CloseAudioDevice();
}

void SND_loadpreset(SND_Preset preset) {
	switch (preset) {
		case WAV_SQUARE: memcpy(SND_wave_data, SND_wav_square, BEEP_WAVE_LEN); return;
		case WAV_TRIANGLE: memcpy(SND_wave_data, SND_wav_triangle, BEEP_WAVE_LEN); return;
		case WAV_SAWTOOTH: memcpy(SND_wave_data, SND_wav_sawtooth, BEEP_WAVE_LEN); return;
		case WAV_SINE: memcpy(SND_wave_data, SND_wav_sine, BEEP_WAVE_LEN); return;
	}
}

void SND_beep(float freq, float duration) {
	__beep_duration = duration;
	__beep_frequency = freq;
}

