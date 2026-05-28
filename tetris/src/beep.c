#include "../include/beep.h"

//	
//	Global Variable Definitions
//

uint8_t SND_wave_data[BEEP_WAVE_LEN] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

float SND_frequency = 0.0f;
float SND_duration = 0.0f;
float SND_volume = 0.0f;


//	
//	Wave Presets
//

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
	0xB0, 0xD9, 0xF5, 0xFF,
	0xF5, 0xD9, 0xB0, 0x7F,
	0x4E, 0x25, 0x09, 0x00,
	0x09, 0x25, 0x4E, 0x7F,
};
const static uint8_t SND_wav_organ[BEEP_WAVE_LEN] = {
	0xFF, 0xC0, 0xC0, 0x80,
	0xC0, 0x80, 0x80, 0x40,
	0xC0, 0x80, 0x80, 0x40,
	0x80, 0x40, 0x40, 0x00
};


//	
//	Internal Global Variable Definitions
//

static AudioStream SND_stream;

#define OSC_BUF_SIZE 1024
static uint8_t __osc_buffer[OSC_BUF_SIZE];
static int __osc_index = 0;

void __callback(void *data, unsigned int size) {
	static float phase = 0.0f;
	for (int i=0; i<size; i++) {

		// Handle basic envelope to avoid popping
		static const float env_delta = 1.0f / (BEEP_ENV_TIME * (float) BEEP_SAMPLE_FREQ);
		if (SND_duration > BEEP_ENV_TIME && SND_volume < 1.0f) SND_volume += env_delta;
		if (SND_volume > 1.0f) SND_volume = 1.0f;

		if (SND_duration <= BEEP_ENV_TIME) SND_volume -= env_delta;
		if (SND_volume <= 0.0f) SND_volume = 0.0f;

		// Create sample from wavetable
		int wi = BEEP_WAVE_LEN * phase;
		float sample = SND_volume * (float) SND_wave_data[wi];
		((uint8_t *) data)[i] = (uint8_t) sample;
		if (__osc_index < OSC_BUF_SIZE) __osc_buffer[__osc_index++] = (uint8_t) sample;

		// Advance wave phase
		if (SND_frequency > 0.0f) phase += 1.0f / ((float) BEEP_SAMPLE_FREQ / SND_frequency);
		if (phase > 1.0f) {
			phase -= 1.0f;
			if (__osc_index >= OSC_BUF_SIZE) __osc_index = 0;
		}

	}
	
	SND_duration -= ((float) size / (float) BEEP_SAMPLE_FREQ);
	if (SND_duration < 0.0f) SND_duration = 0.0f;
}

void SND_init() {
	InitAudioDevice();
	SetMasterVolume(0.30f);

	SND_stream = LoadAudioStream(
		BEEP_SAMPLE_FREQ,
		sizeof(uint8_t) * 8, 1
	);

	SetAudioStreamCallback(SND_stream, &__callback);
	PlayAudioStream(SND_stream);
}

void SND_terminate() {
	StopAudioStream(SND_stream);
	UnloadAudioStream(SND_stream);

	CloseAudioDevice();
}

void SND_loadpreset(SND_Preset preset) {
	switch (preset) {
		case WAV_SQUARE: memcpy(SND_wave_data, SND_wav_square, BEEP_WAVE_LEN); return;
		case WAV_TRIANGLE: memcpy(SND_wave_data, SND_wav_triangle, BEEP_WAVE_LEN); return;
		case WAV_SAWTOOTH: memcpy(SND_wave_data, SND_wav_sawtooth, BEEP_WAVE_LEN); return;
		case WAV_SINE: memcpy(SND_wave_data, SND_wav_sine, BEEP_WAVE_LEN); return;
		case WAV_ORGAN: memcpy(SND_wave_data, SND_wav_organ, BEEP_WAVE_LEN); return;
	}
}

float SND_note_to_freq(uint8_t note) {
	int offset = note - 69;
	return BEEP_FREQ_A4 * pow(BEEP_SEMITONE, offset);
}

void SND_beep(float freq, float duration) {
	SND_duration = duration;
	SND_frequency = freq;
}

void SND_draw_osc(Rectangle scope) {
	DrawRectangleRec(scope, BLACK);
	for (int i=1; i<OSC_BUF_SIZE; i++) {

		Vector2 start = {
			scope.x + ((float) (i-1) / OSC_BUF_SIZE) * scope.width,
			scope.y + scope.height - ((float) __osc_buffer[i-1] / 0xFF) * (scope.height * 0.80f) - (scope.height * 0.10f)
		};
		Vector2 end = {
			scope.x + ((float) i / OSC_BUF_SIZE) * scope.width,
			scope.y + scope.height - ((float) __osc_buffer[i] / 0xFF) * (scope.height * 0.80f) - (scope.height * 0.10f)
		};

		DrawLineEx(start, end, 2, GREEN);
	}
}


